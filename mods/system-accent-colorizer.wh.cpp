// ==WindhawkMod==
// @id              system-accent-colorizer
// @name            System Accent Colorizer
// @description     Customizes text selections, selection boxes, and progress bars to match your accent color.
// @version         1.0.0
// @author          ciizerr
// @github          https://github.com/ciizerr
// @include         *
// @exclude         antigravity.exe
// @compilerOptions -luser32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- enable_highlight_color: true
  $name: Enable Highlight Background Color
- highlight_color: ""
  $name: Highlight Background Color
  $description: Leave empty to use system Accent Color. Or use hexadecimal RGB e.g. 00FF00
- enable_highlight_text_color: true
  $name: Enable Highlight Text Color
- highlight_text_color: "FFFFFF"
  $name: Highlight Text Color
  $description: Color in hexadecimal RGB format e.g. White = FFFFFF
- enable_hotlight_color: true
  $name: Enable Translucent Selection Box Color
- hotlight_color: ""
  $name: Translucent Selection Box Color
  $description: Leave empty to use system Accent Color. Or use hexadecimal RGB e.g. 0066CC
- enable_progress_bar_color: true
  $name: Enable Progress Bar Colorization
  $description: Enable customized disk usage and progress bar colors.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>
#include <string>
#include <array>

enum AccentColorShade {
    SystemAccentColorLight3,
    SystemAccentColorLight2,
    SystemAccentColorLight1,
    SystemAccentColorBase,
    SystemAccentColorDark1,
    SystemAccentColorDark2,
    SystemAccentColorDark3,
    Unused,
    AccentColorCount
};

class AccentPalette {
public:
    std::array<COLORREF, AccentColorCount> Colors;
    BOOL LoadAccentPalette() {
        const LPCWSTR kAccentRegPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent";
        const LPCWSTR kAccentPaletteValue = L"AccentPalette";

        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kAccentRegPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return FALSE;

        BYTE data[32] = {};
        DWORD dataSize = sizeof(data);
        DWORD type = 0;

        if (RegQueryValueExW(hKey, kAccentPaletteValue, nullptr, &type, data, &dataSize) != ERROR_SUCCESS || type != REG_BINARY || dataSize < AccentColorCount * 4) {
            RegCloseKey(hKey);
            return FALSE;
        }

        RegCloseKey(hKey);

        for (int i = 0; i < AccentColorCount; ++i) {
            DWORD color = *reinterpret_cast<DWORD*>(&data[i * 4]);
            Colors[i] = color & 0x00FFFFFF;
        }
        return TRUE;
    }
    AccentPalette() {
        for (auto& c : Colors) c = RGB(0, 120, 215);
        LoadAccentPalette();
    }
};

AccentPalette g_AccentPalette;

// Convert string like "00FF00" to COLORREF
COLORREF ParseColor(const std::wstring& str, COLORREF defaultColor) {
    if (str.length() == 6) {
        try {
            int r = std::stoi(str.substr(0, 2), nullptr, 16);
            int g = std::stoi(str.substr(2, 2), nullptr, 16);
            int b = std::stoi(str.substr(4, 2), nullptr, 16);
            return RGB(r, g, b);
        } catch (...) {
            // Fallback on error
        }
    }
    return defaultColor;
}

COLORREF g_highlightColor = RGB(0, 120, 215);
COLORREF g_highlightTextColor = RGB(255, 255, 255);
COLORREF g_hotlightColor = RGB(0, 102, 204);
HBRUSH g_highlightBrush = nullptr;
HBRUSH g_hotlightBrush = nullptr;

bool g_enableHighlightColor = true;
bool g_enableHighlightTextColor = true;
bool g_enableHotlightColor = true;
bool g_enableProgressBarColor = true;

typedef DWORD(WINAPI *GetSysColor_t)(int nIndex);
GetSysColor_t GetSysColor_Original;

DWORD WINAPI GetSysColor_Hook(int nIndex) {
    if (nIndex == COLOR_HIGHLIGHT && g_enableHighlightColor) {
        return g_highlightColor;
    }
    if (nIndex == COLOR_HIGHLIGHTTEXT && g_enableHighlightTextColor) {
        return g_highlightTextColor;
    }
    if (nIndex == COLOR_HOTLIGHT && g_enableHotlightColor) {
        return g_hotlightColor;
    }
    return GetSysColor_Original(nIndex);
}

typedef HBRUSH(WINAPI *GetSysColorBrush_t)(int nIndex);
GetSysColorBrush_t GetSysColorBrush_Original;

HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    if (nIndex == COLOR_HIGHLIGHT && g_enableHighlightColor) {
        if (!g_highlightBrush) {
            g_highlightBrush = CreateSolidBrush(g_highlightColor);
        }
        return g_highlightBrush;
    }
    if (nIndex == COLOR_HOTLIGHT && g_enableHotlightColor) {
        if (!g_hotlightBrush) {
            g_hotlightBrush = CreateSolidBrush(g_hotlightColor);
        }
        return g_hotlightBrush;
    }
    return GetSysColorBrush_Original(nIndex);
}

std::wstring GetThemeClass(HTHEME hTheme) {
    typedef HRESULT(WINAPI* pGetThemeClass)(HTHEME, LPWSTR, int);
    static auto GetClassName = (pGetThemeClass)GetProcAddress(GetModuleHandleW(L"uxtheme.dll"), MAKEINTRESOURCEA(74));
    if (GetClassName) {
        WCHAR buffer[255] = { 0 };
        GetClassName(hTheme, buffer, 255);
        return std::wstring(buffer);
    }
    return L"";
}

typedef HRESULT(WINAPI *DrawThemeBackground_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect);
DrawThemeBackground_t DrawThemeBackground_Original;

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect) {
    std::wstring themeClass = GetThemeClass(hTheme);
    if (g_enableProgressBarColor && themeClass == L"Progress" && (iPartId == 5 /*PP_FILL*/ || iPartId == 3 /*PP_CHUNK*/)) {
        if (iStateId != 2 /*PBFS_ERROR*/) { // Ignore red error state
            HBRUSH brush = CreateSolidBrush(g_highlightColor);
            FillRect(hdc, pRect, brush);
            DeleteObject(brush);
            return S_OK;
        }
    }
    return DrawThemeBackground_Original(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

typedef HRESULT(WINAPI *DrawThemeBackgroundEx_t)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS* pOptions);
DrawThemeBackgroundEx_t DrawThemeBackgroundEx_Original;

HRESULT WINAPI DrawThemeBackgroundEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS* pOptions) {
    std::wstring themeClass = GetThemeClass(hTheme);
    if (g_enableProgressBarColor && themeClass == L"Progress" && (iPartId == 5 /*PP_FILL*/ || iPartId == 3 /*PP_CHUNK*/)) {
        if (iStateId != 2 /*PBFS_ERROR*/) { // Ignore red error state
            HBRUSH brush = CreateSolidBrush(g_highlightColor);
            FillRect(hdc, pRect, brush);
            DeleteObject(brush);
            return S_OK;
        }
    }
    return DrawThemeBackgroundEx_Original(hTheme, hdc, iPartId, iStateId, pRect, pOptions);
}

void LoadSettings() {
    g_enableHighlightColor = Wh_GetIntSetting(L"enable_highlight_color");
    g_enableHighlightTextColor = Wh_GetIntSetting(L"enable_highlight_text_color");
    g_enableHotlightColor = Wh_GetIntSetting(L"enable_hotlight_color");
    g_enableProgressBarColor = Wh_GetIntSetting(L"enable_progress_bar_color");

    g_AccentPalette.LoadAccentPalette();
    COLORREF accentColor = g_AccentPalette.Colors[SystemAccentColorBase];

    LPCWSTR colorStr = Wh_GetStringSetting(L"highlight_color");
    if (colorStr) {
        g_highlightColor = ParseColor(colorStr, accentColor);
        Wh_FreeStringSetting(colorStr);
    }
    LPCWSTR textColorStr = Wh_GetStringSetting(L"highlight_text_color");
    if (textColorStr) {
        g_highlightTextColor = ParseColor(textColorStr, RGB(255, 255, 255));
        Wh_FreeStringSetting(textColorStr);
    }
    LPCWSTR hotlightColorStr = Wh_GetStringSetting(L"hotlight_color");
    if (hotlightColorStr) {
        g_hotlightColor = ParseColor(hotlightColorStr, accentColor);
        Wh_FreeStringSetting(hotlightColorStr);
    }

    HBRUSH newHighlightBrush = CreateSolidBrush(g_highlightColor);
    HBRUSH oldHighlightBrush = (HBRUSH)InterlockedExchangePointer((PVOID*)&g_highlightBrush, newHighlightBrush);
    if (oldHighlightBrush) DeleteObject(oldHighlightBrush);

    HBRUSH newHotlightBrush = CreateSolidBrush(g_hotlightColor);
    HBRUSH oldHotlightBrush = (HBRUSH)InterlockedExchangePointer((PVOID*)&g_hotlightBrush, newHotlightBrush);
    if (oldHotlightBrush) DeleteObject(oldHotlightBrush);
}

typedef LRESULT(WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_SETTINGCHANGE && lParam != 0 && (ULONG_PTR)lParam > 0xFFFF) {
        if (lstrcmpiW((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
            LoadSettings();
        }
    } else if (Msg == 0x031A /*WM_THEMECHANGED*/ || Msg == 0x0015 /*WM_SYSCOLORCHANGE*/ || Msg == 0x0320 /*WM_DWMCOLORIZATIONCOLORCHANGED*/) {
        LoadSettings();
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

typedef LRESULT(WINAPI *DefWindowProcA_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcA_t DefWindowProcA_Original;
LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_SETTINGCHANGE && lParam != 0 && (ULONG_PTR)lParam > 0xFFFF) {
        if (lstrcmpiA((LPCSTR)lParam, "ImmersiveColorSet") == 0) {
            LoadSettings();
        }
    } else if (Msg == 0x031A /*WM_THEMECHANGED*/ || Msg == 0x0015 /*WM_SYSCOLORCHANGE*/ || Msg == 0x0320 /*WM_DWMCOLORIZATIONCOLORCHANGED*/) {
        LoadSettings();
    }
    return DefWindowProcA_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    Wh_Log(L"System Accent Colorizer: Init");
    LoadSettings();

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        void* pGetSysColor = (void*)GetProcAddress(hUser32, "GetSysColor");
        if (pGetSysColor) {
            Wh_SetFunctionHook(pGetSysColor, (void*)GetSysColor_Hook, (void**)&GetSysColor_Original);
        }

        void* pGetSysColorBrush = (void*)GetProcAddress(hUser32, "GetSysColorBrush");
        if (pGetSysColorBrush) {
            Wh_SetFunctionHook(pGetSysColorBrush, (void*)GetSysColorBrush_Hook, (void**)&GetSysColorBrush_Original);
        }

        void* pDefWindowProcW = (void*)GetProcAddress(hUser32, "DefWindowProcW");
        if (pDefWindowProcW) {
            Wh_SetFunctionHook(pDefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
        }

        void* pDefWindowProcA = (void*)GetProcAddress(hUser32, "DefWindowProcA");
        if (pDefWindowProcA) {
            Wh_SetFunctionHook(pDefWindowProcA, (void*)DefWindowProcA_Hook, (void**)&DefWindowProcA_Original);
        }
    }

    HMODULE hUxTheme = GetModuleHandle(L"uxtheme.dll");
    if (hUxTheme) {
        void* pDrawThemeBackground = (void*)GetProcAddress(hUxTheme, "DrawThemeBackground");
        if (pDrawThemeBackground) {
            Wh_SetFunctionHook(pDrawThemeBackground, (void*)DrawThemeBackground_Hook, (void**)&DrawThemeBackground_Original);
        }

        void* pDrawThemeBackgroundEx = (void*)GetProcAddress(hUxTheme, "DrawThemeBackgroundEx");
        if (pDrawThemeBackgroundEx) {
            Wh_SetFunctionHook(pDrawThemeBackgroundEx, (void*)DrawThemeBackgroundEx_Hook, (void**)&DrawThemeBackgroundEx_Original);
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"System Accent Colorizer: Uninit");
    if (g_highlightBrush) {
        DeleteObject(g_highlightBrush);
        g_highlightBrush = nullptr;
    }
    if (g_hotlightBrush) {
        DeleteObject(g_hotlightBrush);
        g_hotlightBrush = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"System Accent Colorizer: Settings Changed");
    LoadSettings();
}
