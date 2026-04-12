// ==WindhawkMod==
// @id              neko-cat
// @name            Neko Cat
// @description     Adds a desktop pet cat that runs around and follows your mouse
// @version         1.0.0
// @author          ciizerr
// @github          https://github.com/ciizerr
// @include         explorer.exe
// @compilerOptions -lgdiplus -lwinmm -lgdi32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- asset_path: D:\wh-mods\assets\neko-cat
  $name: Asset Path
  $description: Path to the neko-cat folder
- scale: 2
  $name: Cat Scale
  $description: Size multiplier of the cat (1 = 32px, 2 = 64px, etc.)
- speed: 24
  $name: Speed
  $description: Cat's movement speed per tick
- sound: true
  $name: Enable Sound
  $description: Enable cat meows and sleeping sounds
- sound_theme: default
  $name: Sound Theme
  $description: Folder to pull sounds from (assets/neko-cat/sounds/[Theme]/...)
  $options:
  - default: Default
  - memes: Memes
  - cute: Cute
- sleep_sound_interval: 30
  $name: Sleep Sound Interval
  $description: Seconds between sleep snoring sounds
- fps: 60
  $name: Target FPS
  $description: Framerate (for smooth movement)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <cmath>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")

using namespace Gdiplus;

enum NekoState {
    STOP = 0, WASH, SCRATCH, YAWN, SLEEP, AWAKE,
    U_MOVE, D_MOVE, L_MOVE, R_MOVE, UL_MOVE, UR_MOVE, DL_MOVE, DR_MOVE,
    U_CLAW, D_CLAW, L_CLAW, R_CLAW, MAX_STATE
};

enum BehaviorMode {
    CHASE_MOUSE = 0, RUN_AWAY, RANDOM, PACE, RUN_AROUND, FORCED_SLEEP, MAX_BEHAVIOR
};

const int STOP_TIME = 4;
const int WASH_TIME = 10;
const int SCRATCH_TIME = 4;
const int YAWN_TIME = 3;
const int AWAKE_TIME = 3;
const int CLAW_TIME = 10;

const int SPRITE_SIZE = 32;

std::wstring g_assetPath = L"D:\\wh-mods\\assets\\neko-cat";
std::wstring g_soundTheme = L"default";
int g_scale = 2;
int g_speed = 24;
bool g_soundEnabled = true;
int g_sleepSoundInterval = 30;
int g_fps = 60;
bool g_modExit = false;

struct SpriteConfig {
    const wchar_t* files[2];
};

SpriteConfig g_spriteConfigs[MAX_STATE] = {
    { L"awake.png", L"awake.png" }, // STOP
    { L"wash1.png", L"wash2.png" }, // WASH
    { L"scratch1.png", L"scratch2.png" }, // SCRATCH
    { L"yawn1.png", L"yawn2.png" }, // YAWN
    { L"sleep1.png", L"sleep2.png" }, // SLEEP
    { L"awake.png", L"awake.png" }, // AWAKE
    { L"up1.png", L"up2.png" }, // U_MOVE
    { L"down1.png", L"down2.png" }, // D_MOVE
    { L"left1.png", L"left2.png" }, // L_MOVE
    { L"right1.png", L"right2.png" }, // R_MOVE
    { L"upleft1.png", L"upleft2.png" }, // UL_MOVE
    { L"upright1.png", L"upright2.png" }, // UR_MOVE
    { L"downleft1.png", L"downleft2.png" }, // DL_MOVE
    { L"downright1.png", L"downright2.png" }, // DR_MOVE
    { L"upclaw1.png", L"upclaw2.png" }, // U_CLAW
    { L"downclaw1.png", L"downclaw2.png" }, // D_CLAW
    { L"leftclaw1.png", L"leftclaw2.png" }, // L_CLAW
    { L"rightclaw1.png", L"rightclaw2.png" }, // R_CLAW
};

class Neko {
public:
    HWND hwnd = NULL;
    Bitmap* sprites[MAX_STATE][2] = {0};

    int behaviorMode = CHASE_MOUSE;
    int idleThreshold = 6;
    NekoState state = STOP;
    int tickCount = 0;
    int stateCount = 0;
    
    double x = 0, y = 0;
    double logicX = 0, logicY = 0;
    double prevLogicX = 0, prevLogicY = 0;
    double targetX = 0, targetY = 0;
    double oldTargetX = 0, oldTargetY = 0;
    
    int moveDX = 0, moveDY = 0;
    int lastMoveDX = 0, lastMoveDY = 0;

    int boundsWidth = 1920, boundsHeight = 1080;
    
    int mouseX = 0, mouseY = 0;
    bool hasMouseMoved = false;

    double tickAccumulator = 0;
    
    int cornerIndex = 0;
    double ballX = 0, ballY = 0;
    double ballVX = 0, ballVY = 0;
    int actionCount = 0;

    bool isDragging = false;
    ULONGLONG lastSleepSoundTime = 0;
    int sleepSoundPlayCount = 0;

    void LoadSprites() {
        for (int i = 0; i < MAX_STATE; i++) {
            for (int f = 0; f < 2; f++) {
                std::wstring path = g_assetPath + L"\\" + g_spriteConfigs[i].files[f];
                sprites[i][f] = Bitmap::FromFile(path.c_str());
            }
        }
    }

    void PlayAudio(const wchar_t* file, bool loop) {
        if (!g_soundEnabled) return;
        std::wstring path = g_assetPath + L"\\sounds\\" + g_soundTheme + L"\\" + file;
        DWORD flags = SND_ASYNC | SND_FILENAME | SND_NODEFAULT;
        if (loop) flags |= SND_LOOP;
        PlaySoundW(path.c_str(), NULL, flags);
    }
    void StopAudio() {
        PlaySoundW(NULL, NULL, 0);
    }

    void Init() {
        boundsWidth = GetSystemMetrics(SM_CXSCREEN) - SPRITE_SIZE * g_scale;
        boundsHeight = GetSystemMetrics(SM_CYSCREEN) - SPRITE_SIZE * g_scale;
        
        x = rand() % boundsWidth;
        y = rand() % boundsHeight;
        logicX = x; logicY = y;
        prevLogicX = x; prevLogicY = y;
        targetX = x; targetY = y;
        oldTargetX = x; oldTargetY = y;
        
        LoadSprites();

        WNDCLASSW wc = {0};
        wc.lpfnWndProc = NekoWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"NekoCatLayeredWnd";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        
        hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            L"NekoCatLayeredWnd", L"Neko Cat",
            WS_POPUP,
            0, 0, SPRITE_SIZE * g_scale, SPRITE_SIZE * g_scale,
            NULL, NULL, wc.hInstance, NULL
        );
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
        ShowWindow(hwnd, SW_SHOWNA);
    }

    static LRESULT CALLBACK NekoWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        Neko* pThis = (Neko*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (!pThis) return DefWindowProc(hwnd, msg, wp, lp);

        if (msg == WM_NCLBUTTONDOWN || msg == WM_LBUTTONDOWN) {
            if (pThis->behaviorMode != FORCED_SLEEP) {
                if (pThis->state == SLEEP) {
                    pThis->PlayAudio(L"awake.wav", false);
                } else {
                    const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
                    pThis->PlayAudio(idles[rand() % 3], false);
                }
                pThis->SetState(AWAKE);
                pThis->CycleBehavior();
            }
            // Let it fall to DefWindowProc so dragging still functions
        } else if (msg == WM_NCRBUTTONDOWN || msg == WM_RBUTTONDOWN) {
            if (pThis->behaviorMode == FORCED_SLEEP) {
                pThis->behaviorMode = CHASE_MOUSE;
                if (pThis->state == SLEEP || pThis->state == YAWN) {
                    pThis->PlayAudio(L"awake.wav", false);
                } else {
                    const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
                    pThis->PlayAudio(idles[rand() % 3], false);
                }
                pThis->SetState(AWAKE);
            }
            return 0;
        } else if (msg == WM_ENTERSIZEMOVE) {
            pThis->isDragging = true;
        } else if (msg == WM_EXITSIZEMOVE) {
            pThis->isDragging = false;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            pThis->x = rect.left;
            pThis->y = rect.top;
            pThis->logicX = pThis->x;
            pThis->logicY = pThis->y;
            pThis->prevLogicX = pThis->x;
            pThis->prevLogicY = pThis->y;
            
            pThis->behaviorMode = FORCED_SLEEP;
            pThis->SetState(YAWN);
            pThis->oldTargetX = pThis->targetX = pThis->logicX + SPRITE_SIZE * g_scale / 2.0;
            pThis->oldTargetY = pThis->targetY = pThis->logicY + SPRITE_SIZE * g_scale;
        } else if (msg == WM_NCHITTEST) {
            LRESULT hit = DefWindowProc(hwnd, msg, wp, lp);
            if (hit == HTCLIENT) return HTCAPTION;
            return hit;
        }
        return DefWindowProc(hwnd, msg, wp, lp);
    }

    void CycleBehavior() {
        if (behaviorMode == FORCED_SLEEP) return;
        int behaviors[] = {CHASE_MOUSE, RUN_AWAY, RANDOM, PACE, RUN_AROUND};
        int nextMode = CHASE_MOUSE;
        for (int i = 0; i < 5; i++) {
            if (behaviorMode == behaviors[i]) {
                nextMode = behaviors[(i + 1) % 5];
                break;
            }
        }
        behaviorMode = nextMode;
        if (state == SLEEP) SetState(AWAKE);
    }

    void Update() {
        POINT pt;
        GetCursorPos(&pt);
        if (pt.x != mouseX || pt.y != mouseY) {
            mouseX = pt.x; mouseY = pt.y;
            hasMouseMoved = true;
        }
        
        boundsWidth = GetSystemMetrics(SM_CXSCREEN) - SPRITE_SIZE * g_scale;
        boundsHeight = GetSystemMetrics(SM_CYSCREEN) - SPRITE_SIZE * g_scale;

        if (isDragging) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            x = rect.left;
            y = rect.top;
            logicX = x; logicY = y;
            prevLogicX = x; prevLogicY = y;
            return;
        }

        double originalFPS = 5.0;
        tickAccumulator += originalFPS / (double)g_fps;

        while (tickAccumulator >= 1.0) {
            tickAccumulator -= 1.0;
            prevLogicX = logicX;
            prevLogicY = logicY;
            ProcessOriginalTick();
        }

        double t = tickAccumulator;
        x = prevLogicX + (logicX - prevLogicX) * t;
        y = prevLogicY + (logicY - prevLogicY) * t;

        UpdateWindowPosition();
    }

    void ProcessOriginalTick() {
        tickCount++;
        if (tickCount >= 9999) tickCount = 0;
        if (tickCount % 2 == 0) stateCount++;

        switch (behaviorMode) {
            case CHASE_MOUSE: ChaseMouse(); break;
            case RUN_AWAY: RunAwayFromMouse(); break;
            case RANDOM: RunRandomly(); break;
            case PACE: PaceAroundScreen(); break;
            case RUN_AROUND: RunAround(); break;
            case FORCED_SLEEP: ForcedSleep(); break;
        }

        if (state == SLEEP) {
            ULONGLONG now = GetTickCount64();
            if (now - lastSleepSoundTime > (ULONGLONG)g_sleepSoundInterval * 1000) {
                PlayAudio(L"sleep.wav", false);
                lastSleepSoundTime = now;
                sleepSoundPlayCount = 1;
            } else if (sleepSoundPlayCount == 1 && now - lastSleepSoundTime > 3000) {
                PlayAudio(L"sleep.wav", false);
                sleepSoundPlayCount = 2; // done until next interval
            }
        } else {
            sleepSoundPlayCount = 0;
            // random chance to purr/idle noise
            if ((state == STOP || state == WASH) && g_soundEnabled && rand() % 50 == 0) {
                const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
                PlayAudio(idles[rand() % 3], false);
            }
        }
    }

    void ForcedSleep() {
        RunTowards(logicX + SPRITE_SIZE * g_scale / 2.0, logicY + SPRITE_SIZE * g_scale);
    }

    void ChaseMouse() {
        if (!hasMouseMoved) {
            RunTowards(logicX + SPRITE_SIZE * g_scale / 2, logicY + SPRITE_SIZE * g_scale);
            return;
        }
        RunTowards(mouseX, mouseY);
    }

    void RunAwayFromMouse() {
        if (!hasMouseMoved) {
            RunTowards(logicX + SPRITE_SIZE * g_scale / 2, logicY + SPRITE_SIZE * g_scale);
            return;
        }
        int dwLimit = idleThreshold * 16 * g_scale;
        double xdiff = logicX + SPRITE_SIZE * g_scale / 2 - mouseX;
        double ydiff = logicY + SPRITE_SIZE * g_scale / 2 - mouseY;
        
        if (abs(xdiff) < dwLimit && abs(ydiff) < dwLimit) {
            double dLength = sqrt(xdiff * xdiff + ydiff * ydiff);
            double tx, ty;
            if (dLength != 0) {
                tx = logicX + (xdiff / dLength) * dwLimit;
                ty = logicY + (ydiff / dLength) * dwLimit;
            } else {
                tx = ty = 32 * g_scale;
            }
            RunTowards(tx, ty);
            if (state == AWAKE) CalcDirection(tx - logicX, ty - logicY);
        } else {
            RunTowards(targetX, targetY);
        }
    }

    void RunRandomly() {
        if (state == SLEEP) actionCount++;
        if (actionCount > idleThreshold * 10) {
            actionCount = 0;
            targetX = rand() % boundsWidth;
            targetY = rand() % boundsHeight;
            RunTowards(targetX, targetY);
        } else {
            RunTowards(targetX, targetY);
        }
    }

    void PaceAroundScreen() {
        if (lastMoveDX == 0 && lastMoveDY == 0) {
            cornerIndex = (cornerIndex + 1) % 4;
        }
        int sz = SPRITE_SIZE * g_scale;
        double corners[4][2] = {
            { sz + sz/2.0, (double)(sz + sz) },
            { sz + sz/2.0, (double)(boundsHeight - sz + sz) },
            { boundsWidth - sz + sz/2.0, (double)(boundsHeight - sz + sz) },
            { boundsWidth - sz + sz/2.0, (double)(sz + sz) }
        };
        RunTowards(corners[cornerIndex][0], corners[cornerIndex][1]);
    }

    void RunAround() {
        double bbox = g_speed * 8 * g_scale;
        if (ballX == 0 && ballY == 0) {
            ballX = rand() % (boundsWidth - (int)bbox);
            ballY = rand() % (boundsHeight - (int)bbox);
            ballVX = ((rand() % 2) ? 1 : -1) * (g_speed / 2.0) + 1;
            ballVY = ((rand() % 2) ? 1 : -1) * (g_speed / 2.0) + 1;
        }
        ballX += ballVX;
        ballY += ballVY;
        if (ballX < bbox) {
            if (ballX > 0) ballVX++; else ballVX = -ballVX;
        } else if (ballX > boundsWidth - bbox) {
            if (ballX < boundsWidth) ballVX--; else ballVX = -ballVX;
        }
        if (ballY < bbox) {
            if (ballY > 0) ballVY++; else ballVY = -ballVY;
        } else if (ballY > boundsHeight - bbox) {
            if (ballY < boundsHeight) ballVY--; else ballVY = -ballVY;
        }
        RunTowards(ballX, ballY);
    }

    void SetState(NekoState newState) {
        if (state == SLEEP && newState != SLEEP) {
            StopAudio();
        }

        tickCount = 0;
        stateCount = 0;
        state = newState;
    }

    void CalcDirection(double dx, double dy) {
        NekoState newState;
        if (dx == 0 && dy == 0) {
            newState = STOP;
        } else {
            double length = sqrt(dx*dx + dy*dy);
            double sinTheta = -dy / length; // inverted Y
            const double sinPiPer8 = 0.3826834323651;
            const double sinPiPer8Times3 = 0.9238795325113;

            if (dx > 0) {
                if (sinTheta > sinPiPer8Times3) newState = U_MOVE;
                else if (sinTheta > sinPiPer8) newState = UR_MOVE;
                else if (sinTheta > -sinPiPer8) newState = R_MOVE;
                else if (sinTheta > -sinPiPer8Times3) newState = DR_MOVE;
                else newState = D_MOVE;
            } else {
                if (sinTheta > sinPiPer8Times3) newState = U_MOVE;
                else if (sinTheta > sinPiPer8) newState = UL_MOVE;
                else if (sinTheta > -sinPiPer8) newState = L_MOVE;
                else if (sinTheta > -sinPiPer8Times3) newState = DL_MOVE;
                else newState = D_MOVE;
            }
        }
        if (state != newState) SetState(newState);
    }

    void RunTowards(double tx, double ty) {
        oldTargetX = targetX; oldTargetY = targetY;
        targetX = tx; targetY = ty;
        
        int sz = SPRITE_SIZE * g_scale;
        double dx = tx - logicX - sz/2.0;
        double dy = ty - logicY - sz; 
        double dist = sqrt(dx*dx + dy*dy);

        double actualSpeed = g_speed * g_scale;

        if (dist != 0) {
            if (dist <= actualSpeed) {
                moveDX = (int)dx; moveDY = (int)dy;
            } else {
                moveDX = (int)(actualSpeed * dx / dist);
                moveDY = (int)(actualSpeed * dy / dist);
            }
        } else {
            moveDX = moveDY = 0;
        }

        lastMoveDX = moveDX; lastMoveDY = moveDY;

        bool moveStart = !(
            oldTargetX >= targetX - idleThreshold && oldTargetX <= targetX + idleThreshold &&
            oldTargetY >= targetY - idleThreshold && oldTargetY <= targetY + idleThreshold
        );

        switch (state) {
            case STOP:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= STOP_TIME) {
                    if (moveDX < 0 && logicX <= 0) SetState(L_CLAW);
                    else if (moveDX > 0 && logicX >= boundsWidth) SetState(R_CLAW);
                    else if (moveDY < 0 && logicY <= 0) SetState(U_CLAW);
                    else if (moveDY > 0 && logicY >= boundsHeight) SetState(D_CLAW);
                    else SetState(WASH);
                }
                break;
            case WASH:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= WASH_TIME) SetState(SCRATCH);
                break;
            case SCRATCH:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= SCRATCH_TIME) SetState(YAWN);
                break;
            case YAWN:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= YAWN_TIME) SetState(SLEEP);
                break;
            case SLEEP:
                if (moveStart) SetState(AWAKE);
                break;
            case AWAKE:
                if (stateCount >= AWAKE_TIME + rand() % 20) CalcDirection(moveDX, moveDY);
                break;
            case U_MOVE: case D_MOVE: case L_MOVE: case R_MOVE:
            case UL_MOVE: case UR_MOVE: case DL_MOVE: case DR_MOVE: {
                double nx = logicX + moveDX;
                double ny = logicY + moveDY;
                bool wasOutside = nx <= 0 || nx >= boundsWidth || ny <= 0 || ny >= boundsHeight;
                CalcDirection(moveDX, moveDY);
                
                nx = fmax(0.0, fmin((double)boundsWidth, nx));
                ny = fmax(0.0, fmin((double)boundsHeight, ny));
                bool notMoved = nx == logicX && ny == logicY;
                if (wasOutside && notMoved) SetState(STOP);
                else { logicX = nx; logicY = ny; }
                break;
            }
            case U_CLAW: case D_CLAW: case L_CLAW: case R_CLAW:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= CLAW_TIME) SetState(SCRATCH);
                break;
            default:
                SetState(STOP);
                break;
        }
    }

    void UpdateWindowPosition() {
        int frameObj = 0;
        if (state == SLEEP) frameObj = (tickCount >> 2) & 1;
        else frameObj = tickCount & 1;

        Bitmap* bmp = sprites[state][frameObj];
        if (!bmp || bmp->GetLastStatus() != Ok) return;

        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        int outSize = SPRITE_SIZE * g_scale;

        HBITMAP hbmMem;
        BITMAPINFO bi = {0};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = outSize;
        bi.bmiHeader.biHeight = -outSize;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits;
        hbmMem = CreateDIBSection(hdcMem, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
        
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

        Graphics g(hdcMem);
        g.SetInterpolationMode(InterpolationModeNearestNeighbor);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.Clear(Color(0, 0, 0, 0));
        g.DrawImage(bmp, Rect(0, 0, outSize, outSize), 0, 0, SPRITE_SIZE, SPRITE_SIZE, UnitPixel);

        POINT ptSrc = {0, 0};
        POINT ptDest = { (LONG)round(x), (LONG)round(y) };
        SIZE winSize = { outSize, outSize };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        UpdateLayeredWindow(hwnd, hdcScreen, &ptDest, &winSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
    }

    ~Neko() {
        if (hwnd) DestroyWindow(hwnd);
        for (int i = 0; i < MAX_STATE; i++) {
            for (int f = 0; f < 2; f++) {
                if (sprites[i][f]) delete sprites[i][f];
            }
        }
    }
};

Neko* g_pNeko = nullptr;

DWORD WINAPI NekoThread(LPVOID param) {
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"WindhawkNekoCatMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(hMutex);
        return 0; // Already running in another explorer
    }

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    g_pNeko = new Neko();
    g_pNeko->Init();

    int intervalMs = 1000 / g_fps;

    MSG msg;
    while (!g_modExit) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_modExit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (g_modExit) break;
        
        g_pNeko->Update();
        Sleep(intervalMs);
    }

    g_pNeko->StopAudio();
    delete g_pNeko;
    g_pNeko = nullptr;

    GdiplusShutdown(gdiplusToken);

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
}

HANDLE hThread = NULL;

void LoadSettings() {
    PCWSTR pszPath = Wh_GetStringSetting(L"asset_path");
    g_assetPath = pszPath;
    Wh_FreeStringSetting(pszPath);

    PCWSTR pszTheme = Wh_GetStringSetting(L"sound_theme");
    g_soundTheme = pszTheme;
    Wh_FreeStringSetting(pszTheme);

    g_scale = Wh_GetIntSetting(L"scale");
    g_speed = Wh_GetIntSetting(L"speed");
    g_soundEnabled = Wh_GetIntSetting(L"sound") != 0;
    g_sleepSoundInterval = Wh_GetIntSetting(L"sleep_sound_interval");
    g_fps = Wh_GetIntSetting(L"fps");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_pNeko) {
        // Force bounds recalculation
        g_pNeko->boundsWidth = GetSystemMetrics(SM_CXSCREEN) - SPRITE_SIZE * g_scale;
        g_pNeko->boundsHeight = GetSystemMetrics(SM_CYSCREEN) - SPRITE_SIZE * g_scale;
    }
}

BOOL Wh_ModInit() {
    LoadSettings();
    
    // Create Neko thread to not block explorer
    g_modExit = false;
    hThread = CreateThread(NULL, 0, NekoThread, NULL, 0, NULL);
    
    return TRUE;
}

void Wh_ModUninit() {
    g_modExit = true;
    if (hThread) {
        WaitForSingleObject(hThread, 3000); // 3 seconds timeout
        CloseHandle(hThread);
        hThread = NULL;
    }
}
