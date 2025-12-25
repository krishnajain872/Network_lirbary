# scripts/win_setup_dev.ps1
# Helper script for Windows users to prepare for Dev Containers

Write-Host "Checking prerequisites for Network Library Dev Container..." -ForegroundColor Cyan

# 1. Check Docker
if (Get-Command docker -ErrorAction SilentlyContinue) {
    Write-Host "[OK] Docker is installed." -ForegroundColor Green
    $dockerVersion = docker --version
    Write-Host "     $dockerVersion" -ForegroundColor Gray
} else {
    Write-Host "[FAIL] Docker is NOT installed." -ForegroundColor Red
    Write-Host "       Please install Docker Desktop for Windows: https://www.docker.com/products/docker-desktop"
    exit 1
}

# 2. Check VSCode
if (Get-Command code -ErrorAction SilentlyContinue) {
    Write-Host "[OK] VSCode is installed." -ForegroundColor Green
} else {
    Write-Host "[WARN] VSCode CLI 'code' not found in PATH." -ForegroundColor Yellow
    Write-Host "       Make sure VSCode is installed."
}

Write-Host "`nInstructions:" -ForegroundColor Cyan
Write-Host "1. Ensure Docker Desktop is running."
Write-Host "2. Install the 'Dev Containers' extension in VSCode (ms-vscode-remote.remote-containers)."
Write-Host "3. Open this folder in VSCode."
Write-Host "4. Click 'Reopen in Container' when prompted, or run the command from the palette (F1)."
Write-Host "`nHappy Coding!" -ForegroundColor Green
