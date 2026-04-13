# ==============================
# Safe PowerShell Script Template
# ==============================

Clear-Host

# -------- FUNCTIONS --------
function Write-Header {
    Write-Host ""
    Write-Host "=====================================" -ForegroundColor Cyan
    Write-Host "   My Script Title" -ForegroundColor Cyan
    Write-Host "=====================================" -ForegroundColor Cyan
    Write-Host ""
}

function Write-Step {
    param (
        [string]$Text
    )
    Write-Host ""
    Write-Host ">> $Text" -ForegroundColor Yellow
}

function Write-Success {
    param ([string]$Text)
    Write-Host "OK: $Text" -ForegroundColor Green
}

function Write-WarningMsg {
    param ([string]$Text)
    Write-Host "WARNING: $Text" -ForegroundColor Yellow
}

function Write-ErrorMsg {
    param ([string]$Text)
    Write-Host "ERROR: $Text" -ForegroundColor Red
}

# -------- ADMIN CHECK --------
function Test-Admin {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

# -------- MAIN --------
Write-Header

Write-Step "Checking permissions..."

if (-not (Test-Admin)) {
    Write-ErrorMsg "Please run as Administrator"
    pause
    exit
}

Write-Success "Admin privileges confirmed"

# -------- EXAMPLE TASK --------
Write-Step "Running example task..."

Start-Sleep -Seconds 1
Write-Success "Task completed"

# -------- PROGRESS BAR EXAMPLE --------
Write-Step "Simulating progress..."

for ($i = 1; $i -le 100; $i += 20) {
    Write-Progress -Activity "Processing" -PercentComplete $i
    Start-Sleep -Milliseconds 200
}

Write-Progress -Activity "Processing" -Completed
Write-Success "Done"

# -------- FINISH --------
Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host " Script Completed Successfully" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""

pause