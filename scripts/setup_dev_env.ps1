# scripts/setup_dev_env.ps1
# Sets up the Windows development environment for Network Library

Write-Host "Setting up Network Library Development Environment..." -ForegroundColor Cyan

# 1. Check for Docker
if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Error "Docker is not installed or not in your PATH. Please install Docker Desktop for Windows."
    exit 1
}

Write-Host "Docker is present." -ForegroundColor Green

# 2. Move to repository root (IMPORTANT)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot  = Resolve-Path "$ScriptDir\.."

Set-Location $RepoRoot
Write-Host "Changed directory to repo root: $RepoRoot" -ForegroundColor DarkGray

# 3. Build the Dev Container Image
Write-Host "Building Dev Container Image (this may take a while)..." -ForegroundColor Yellow

docker build `
    -f .devcontainer/Dockerfile `
    -t network-lib-dev `
    .

if ($LASTEXITCODE -eq 0) {
    Write-Host "Dev Image built successfully!" -ForegroundColor Green
} else {
    Write-Error "Failed to build Docker image."
    exit 1
}

# 4. Instructions
Write-Host "`nSetup Complete!" -ForegroundColor Cyan
Write-Host "--------------------------------------------------------"
Write-Host "OPTION 1: VS Code Dev Containers (Recommended)"
Write-Host "   1. Open this folder in VS Code."
Write-Host "   2. Press F1 → 'Dev Containers: Reopen in Container'."
Write-Host "--------------------------------------------------------"
Write-Host "OPTION 2: Manual SSH Connection"

$dockerCmd = @"
docker run -d -p 2222:22 --name network-dev `
  --cap-add=SYS_PTRACE `
  --security-opt seccomp=unconfined `
  -v "${RepoRoot}:/workspaces/network-library" `
  network-lib-dev
"@

Write-Host $dockerCmd
Write-Host "Connect via SSH (password: root):"
Write-Host "ssh -p 2222 root@localhost"
Write-Host "--------------------------------------------------------"
Write-Host "Happy Coding!" -ForegroundColor Cyan