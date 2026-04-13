# install-config.ps1
# Windhawk Configuration Auto-Installer
# Optimized for professional UI/UX - Tabular Selection - Browser Redirection

# --- Environment Setup ---
$ErrorActionPreference = "Stop"
$RepoBaseUrl = "https://raw.githubusercontent.com/ciizerr/wh-mods/main"
$ConfigRemoteDir = "$RepoBaseUrl/Config"
$WindhawkRegistryPath = "HKLM:\SOFTWARE\Windhawk\Engine\Mods"
$GithubModsUrl = "https://github.com/ciizerr/wh-mods/tree/main/mods"

# --- UI Helper Functions ---
function Draw-Header {
    Clear-Host
    $line = "=" * 58
    Write-Host " +$line+" -ForegroundColor Gray
    Write-Host " |" -NoNewline; Write-Host "     WINDHAWK CONFIGURATION AUTO-INSTALLER          " -ForegroundColor Cyan -NoNewline; Write-Host "|" -ForegroundColor Gray
    Write-Host " |" -NoNewline; Write-Host "     Source: github.com/ciizerr/wh-mods             " -ForegroundColor DarkGray -NoNewline; Write-Host "|" -ForegroundColor Gray
    Write-Host " +$line+" -ForegroundColor Gray
    Write-Host ""
}

function Write-Info { param($msg) Write-Host " [INFO] $msg" -ForegroundColor Cyan }
function Write-Success { param($msg) Write-Host " [OK]   $msg" -ForegroundColor Green }
function Write-WarningMsg { param($msg) Write-Host " [WARN] $msg" -ForegroundColor Yellow }
function Write-ErrorMsg { param($msg) Write-Host " [ERR]  $msg" -ForegroundColor Red }

# --- Admin Check ---
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Draw-Header
    Write-ErrorMsg "Administrative privileges required."
    Write-Host " Please restart your terminal as Administrator."
    Write-Host ""
    exit 1
}

# --- Dependencies (Required but no config) ---
$RequiredMods = @(
    @{ Id = "taskbar-content-presenter-injector"; Url = "https://windhawk.net/mods/taskbar-content-presenter-injector" }
)

# --- Presets Collection (Mapped to YAML files & Store URLs) ---
$Presets = @(
    @{ File = "windows 11 taskbar styler.yaml"; Id = "windows-11-taskbar-styler"; Url = "https://windhawk.net/mods/windows-11-taskbar-styler" }
    @{ File = "windows 11 notification center styler.yaml"; Id = "windows-11-notification-center-styler"; Url = "https://windhawk.net/mods/windows-11-notification-center-styler" }
    @{ File = "windows 11 start menu styler.yaml"; Id = "windows-11-start-menu-styler"; Url = "https://windhawk.net/mods/windows-11-start-menu-styler" }
    @{ File = "windows 11 start menu styler - Fork(secHealthUi.exe).yaml"; Id = "local@windows-11-start-menu-styler-fork"; Url = $GithubModsUrl }
    @{ File = "taskbar clock customisation.yaml"; Id = "taskbar-clock-customization"; Url = "https://windhawk.net/mods/taskbar-clock-customization" }
    @{ File = "vlc discord rich presence.yaml"; Id = "vlc-discord-rpc"; Url = "https://windhawk.net/mods/vlc-discord-rpc" }
    @{ File = "resource redirect.yaml"; Id = "icon-resource-redirect"; Url = "https://windhawk.net/mods/icon-resource-redirect" }
)

Draw-Header

# --- Step 1: Install Mods Check ---
Write-Info "Scanning system for installed Windhawk mods..."
$MissingMods = New-Object System.Collections.Generic.List[Object]
foreach ($item in ($RequiredMods + $Presets)) {
    if (-not (Test-Path (Join-Path $WindhawkRegistryPath $item.Id))) { $MissingMods.Add($item) }
}

if ($MissingMods.Count -gt 0) {
    Write-WarningMsg "Detected $($MissingMods.Count) missing or required mod(s)."
    Write-Host " " + ("-" * 40) -ForegroundColor Gray
    foreach ($m in $MissingMods) { Write-Host " - $($m.Id)" -ForegroundColor DarkGray }
    Write-Host " " + ("-" * 40) -ForegroundColor Gray
    
    Write-Host ""
    $OpenBrowser = Read-Host " Open Windhawk.net/GitHub to install missing mods? (y/n/q to quit) [n]"
    if ($OpenBrowser -eq 'q') { Write-Host "`n Aborted."; exit 0 }
    if ($OpenBrowser -eq 'y') {
        foreach ($m in $MissingMods) {
            if ($m.Id.StartsWith("local@")) {
                Write-WarningMsg "Mod '$($m.Id)' is a custom fork. Please install it manually from your local repository or fork."
            } else {
                Write-Info "Opening browser for: $($m.Id)"
                Start-Process $m.Url
                Start-Sleep -Milliseconds 200
            }
        }
        Write-Host ""
        Read-Host " [!] Install mods then press Enter to continue (or Ctrl+C to quit)"
    }
} else {
    Write-Success "All target and required mods are currently installed.`n"
}

# --- Step 2: Selective Configuration (Tabular UI) ---
Write-Host " CONFIGURATION PRESETS:" -ForegroundColor Yellow
Write-Host " #   Mod ID                                  Status" -ForegroundColor Gray
Write-Host " --  --------------------------------------  ----------" -ForegroundColor Gray

for ($i = 0; $i -lt $Presets.Count; $i++) {
    $item = $Presets[$i]; $modId = $item.Id
    $isInstalled = Test-Path (Join-Path $WindhawkRegistryPath $modId)
    $status = if ($isInstalled) { "Installed" } else { "Missing" }
    $color = if ($isInstalled) { "Gray" } else { "DarkGray" }
    Write-Host " $(($i + 1).ToString().PadRight(3))" -NoNewline -ForegroundColor White
    Write-Host "$($modId.PadRight(40))" -NoNewline -ForegroundColor $color
    Write-Host "$status" -ForegroundColor $color
}

Write-Host ""
Write-Host " Options: Enter numbers (ex: 1,3), 'all', or 'q' to quit" -ForegroundColor Gray
$Selection = Read-Host " Choice [all]"
if ($Selection -eq "q") { Write-Host "`n Aborted."; exit 0 }
if ([string]::IsNullOrWhiteSpace($Selection)) { $Selection = "all" }

# --- Selection Parsing ---
$SelectedIndices = @()
if ($Selection -eq "all") {
    for ($i = 0; $i -lt $Presets.Count; $i++) { $SelectedIndices += $i }
} else {
    $parts = $Selection -split ","
    $val = 0
    foreach ($p in $parts) {
        if ([int]::TryParse($p.Trim(), [ref]$val)) {
            $idx = $val - 1
            if ($idx -ge 0 -and $idx -lt $Presets.Count) { $SelectedIndices += $idx }
        }
    }
}

if ($SelectedIndices.Count -eq 0) { Write-ErrorMsg "Invalid selection."; exit 0 }

Write-Host "`n Starting Deployment Process..." -ForegroundColor Cyan
Write-Host ("=" * 58) -ForegroundColor DarkGray

$DeployResults = New-Object System.Collections.Generic.List[PSObject]

function Flatten-Yaml {
    param([string[]]$Lines)
    $Res = @{}
    $PathStack = New-Object System.Collections.Generic.List[string]
    $IndentStack = New-Object System.Collections.Generic.List[int]
    $ListIndices = @{}
    foreach ($line in $Lines) {
        if ([string]::IsNullOrWhiteSpace($line) -or $line.Trim().StartsWith("#")) { continue }
        $indent = $line.Length - $line.TrimStart().Length
        $trimmed = $line.Trim()
        while ($IndentStack.Count -gt 0 -and $indent -le $IndentStack[$IndentStack.Count - 1]) {
             if ($trimmed.StartsWith("- ") -and $indent -eq $IndentStack[$IndentStack.Count - 1]) { break }
             $PathStack.RemoveAt($PathStack.Count - 1); $IndentStack.RemoveAt($IndentStack.Count - 1)
        }
        if ($trimmed.StartsWith("- ")) {
            $itemContent = $trimmed.Substring(2).Trim()
            $parentPath = ($PathStack -join ".")
            if (-not $ListIndices.ContainsKey($parentPath)) { $ListIndices[$parentPath] = 0 }
            $index = $ListIndices[$parentPath]; $nodePath = "$parentPath[$index]"
            if ($itemContent -match "^([^:]+):\s*(.*)$") {
                $PathStack.Add($nodePath); $IndentStack.Add($indent); $ListIndices[$parentPath]++
                $subKey = $Matches[1].Trim(); $subVal = $Matches[2].Trim()
                if ($subVal -ne "" -and $subVal -ne "''") { $Res["$nodePath.$subKey"] = $subVal }
            } else { $Res[$nodePath] = $itemContent; $ListIndices[$parentPath]++ }
        } elseif ($trimmed -match "^([^:]+):\s*(.*)$") {
            $key = $Matches[1].Trim(); $val = $Matches[2].Trim()
            if ($val -eq "" -or $val -eq "''") { $PathStack.Add($key); $IndentStack.Add($indent) }
            else { $path = ($PathStack -join "."); $fullKey = if ($path -eq "") { $key } else { "$path.$key" }; $Res[$fullKey] = $val }
        }
    }
    return $Res
}

foreach ($idx in $SelectedIndices) {
    $item = $Presets[$idx]; $modId = $item.Id
    $fileUrl = "$ConfigRemoteDir/" + $item.File.Replace(" ", "%20")
    $status = "Success"
    Write-Host " -> Setting up: $modId..." -NoNewline -ForegroundColor Gray
    try {
        $ModKey = Join-Path $WindhawkRegistryPath $modId
        $SettingsKey = Join-Path $ModKey "Settings"
        if (-not (Test-Path $ModKey)) { $status = "Skipped"; Write-Host " SKIPPED" -ForegroundColor Yellow } 
        else {
            $YamlContent = Invoke-RestMethod -Uri $fileUrl -ErrorAction Stop
            if (Test-Path $SettingsKey) { Remove-Item $SettingsKey -Recurse -Force }
            New-Item $SettingsKey -Force | Out-Null
            $Flattened = Flatten-Yaml -Lines ($YamlContent -split "`r?`n")
            foreach ($entry in $Flattened.GetEnumerator()) {
                $val = $entry.Value -replace "^['""]|['""]$", ""
                New-ItemProperty -Path $SettingsKey -Name $entry.Key -Value $val -PropertyType String -Force | Out-Null
            }
            $unixNow = [int][double]::Parse((Get-Date -UFormat %s))
            Set-ItemProperty $ModKey "SettingsChangeTime" $unixNow -Force
            Write-Host " DONE" -ForegroundColor Green
        }
    } catch { $status = "Failed"; Write-Host " FAILED" -ForegroundColor Red }
    $DeployResults.Add([PSCustomObject]@{ Preset = $modId; Status = $status })
}

Write-Host ("=" * 58) -ForegroundColor DarkGray
Write-Host "`n FINAL SUMMARY:" -ForegroundColor Yellow
$DeployResults | Format-Table -AutoSize | Out-String | Write-Host -ForegroundColor Gray
Write-Info "Finalizing: Restarting Windhawk Engine..."
$WindhawkPath = "C:\Program Files\Windhawk\windhawk.exe"
if (Test-Path $WindhawkPath) { Start-Process $WindhawkPath "-restart -tray-only"; Write-Success "Windhawk restarted successfully." }
Write-Host "`n [COMPLETED]" -ForegroundColor Green -NoNewline; Write-Host " Windhawk environment is now up to date." -ForegroundColor Gray
Write-Host ""