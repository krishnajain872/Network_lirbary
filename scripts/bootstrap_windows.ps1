<#
.SYNOPSIS
    Automated Setup Script for Network Library Development on Windows.
.DESCRIPTION
    This script prepares a Windows machine for developing the Network Library.
    It performs the following:
    1. Checks for Administrator privileges.
    2. Installs Docker Desktop (via winget) if missing.
    3. Installs VS Code (via winget) if missing.
    4. Installs the VS Code Dev Containers extension.
    5. Launches VS Code in the project directory.
#>

# Ensure script is run as Administrator
if (!([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Warning "This script requires Administrator privileges to install software."
    Write-Warning "Please right-click and select 'Run as Administrator'."
    Start-Process powershell.exe "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    exit
}

$ErrorActionPreference = "Stop"

function Log-Info($Message) {
    Write-Host "[INFO] $Message" -ForegroundColor Cyan
}

function Log-Success($Message) {
    Write-Host "[OK]   $Message" -ForegroundColor Green
}

function Log-Warn($Message) {
    Write-Host "[WARN] $Message" -ForegroundColor Yellow
}

function Install-WingetPackage {
    param($Id, $Name)
    if (winget list -e --id $Id) {
        Log-Success "$Name is already installed."
    } else {
        Log-Info "Installing $Name..."
        winget install -e --id $Id --accept-package-agreements --accept-source-agreements
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to install $Name"
        }
        Log-Success "$Name installed successfully."
    }
}

Log-Info "Starting Network Library Environment Setup..."

# 1. Check Winget
if (!(Get-Command winget -ErrorAction SilentlyContinue)) {
    throw "Winget (App Installer) is not found. Please update Windows or install App Installer from Microsoft Store."
}

# 2. Install Docker Desktop
Install-WingetPackage "Docker.DockerDesktop" "Docker Desktop"

# 3. Install VS Code
Install-WingetPackage "Microsoft.VisualStudioCode" "Visual Studio Code"

# 4. Refresh Environment Variables (to find code/docker commands without restart)
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

# 5. Check Docker Service
Log-Info "Checking Docker status..."
if (!(Get-Process "Docker Desktop" -ErrorAction SilentlyContinue)) {
    Log-Warn "Docker Desktop is not running. Starting it..."
    & "C:\Program Files\Docker\Docker\Docker Desktop.exe"
    Log-Info "Waiting for Docker to start (this may take a minute)..."
    # Simple loop to wait for docker pipe
    $retries = 30
    while ($retries -gt 0) {
        if (docker info 2>$null) { break }
        Start-Sleep -Seconds 2
        $retries--
    }
    if ($retries -eq 0) {
        Log-Warn "Docker might still be starting. Please ensure it is running manually if next steps fail."
    }
}

# 6. Install VS Code Extension
Log-Info "Installing Dev Containers extension..."
if (Get-Command code -ErrorAction SilentlyContinue) {
    code --install-extension ms-vscode-remote.remote-containers --force
    Log-Success "Dev Containers extension installed."
} else {
    Log-Warn "VS Code CLI 'code' not found. You may need to restart your terminal."
}

# 7. Launch
$projectPath = Resolve-Path "$PSScriptRoot\.."
Log-Success "Setup Complete!"
Log-Info "Opening project in VS Code..."
Log-Info "When VS Code opens, click 'Reopen in Container' (bottom right) to start the Linux environment."

if (Get-Command code -ErrorAction SilentlyContinue) {
    code $projectPath
} else {
    Write-Host "Please open VS Code manually and select this folder: $projectPath"
}

Pause
