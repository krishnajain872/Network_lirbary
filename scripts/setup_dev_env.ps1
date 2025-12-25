# scripts/setup_dev_env.ps1
# Sets up the Windows development environment for Network Library

Write-Host "Setting up Network Library Development Environment..." -ForegroundColor Cyan

# 1. Check for Docker
if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Error "Docker is not installed or not in your PATH. Please install Docker Desktop for Windows."
    exit 1
}

Write-Host "Docker is present." -ForegroundColor Green

# 2. Build the Dev Container Image
Write-Host "Building Dev Container Image (this may take a while)..." -ForegroundColor Yellow
# We build from the root directory context, using the .devcontainer/Dockerfile
# Tagging it as network-lib-dev for easy manual usage
docker build -f .devcontainer/Dockerfile -t network-lib-dev .

if ($LASTEXITCODE -eq 0) {
    Write-Host "Dev Image built successfully!" -ForegroundColor Green
} else {
    Write-Error "Failed to build Docker image."
    exit 1
}

# 3. Instructions
Write-Host "`nSetup Complete!" -ForegroundColor Cyan
Write-Host "--------------------------------------------------------"
Write-Host "OPTION 1: VS Code Dev Containers (Recommended)"
Write-Host "   1. Open this folder in VS Code."
Write-Host "   2. Press F1, type 'Dev Containers: Reopen in Container'."
Write-Host "   3. VS Code will automatically set up the environment."
Write-Host "--------------------------------------------------------"
Write-Host "OPTION 2: Manual SSH Connection"
Write-Host "   1. Run the container:"
Write-Host "      docker run -d -p 2222:22 --name network-dev --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v ${PWD}:/workspaces/network-library network-lib-dev"
Write-Host "   2. Connect via SSH (Password: root):"
Write-Host "      ssh -p 2222 root@localhost"
Write-Host "--------------------------------------------------------"
