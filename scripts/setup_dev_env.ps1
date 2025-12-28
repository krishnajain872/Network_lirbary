# scripts/setup_dev_env.ps1
# Sets up the Windows development environment for Network Library

#Requires -Version 5.1

[CmdletBinding()]
param(
    [switch]$SkipImageBuild,
    [switch]$NoBuildCache,
    [switch]$StartOnly,
    [switch]$UseHost  # Use host network instead of port mapping
)

$ErrorActionPreference = "Stop"

function Write-Step {
    param([string]$Message)
    Write-Host "`n▶ $Message" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Info {
    param([string]$Message)
    Write-Host "  $Message" -ForegroundColor DarkGray
}

function Write-Warning-Custom {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

# =============================================================================
# ENVIRONMENT CHECKS
# =============================================================================

Write-Step "Checking prerequisites..."

if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Error "Docker is not installed or not in your PATH."
    exit 1
}

try {
    docker ps | Out-Null
    Write-Success "Docker is running"
} catch {
    Write-Error "Docker daemon is not running."
    exit 1
}

$dockerVersion = docker version --format '{{.Server.Version}}' 2>$null
Write-Info "Docker version: $dockerVersion"

$env:DOCKER_BUILDKIT = "1"
$env:COMPOSE_DOCKER_CLI_BUILD = "1"
Write-Success "BuildKit enabled"

# =============================================================================
# MOVE TO REPO ROOT
# =============================================================================

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot  = Resolve-Path "$ScriptDir\.."
Push-Location $RepoRoot

Write-Success "Repository root: $RepoRoot"

# =============================================================================
# START-ONLY MODE
# =============================================================================

if ($StartOnly) {
    Write-Step "Start-only mode enabled"
    if (-not (docker images -q network-lib-dev:latest)) {
        Write-Error "Image network-lib-dev:latest not found."
        exit 1
    }
    $SkipImageBuild = $true
}

# =============================================================================
# IMAGE BUILD
# =============================================================================

if (-not $SkipImageBuild) {
    Write-Step "Building dev container image..."

    $buildArgs = @(
        "build",
        "-f", ".devcontainer/Dockerfile",
        "-t", "network-lib-dev:latest",
        "--build-arg", "BUILDKIT_INLINE_CACHE=1"
    )

    if ($NoBuildCache) {
        $buildArgs += "--no-cache"
        Write-Warning-Custom "Clean build requested"
    }

    $buildArgs += "."

    & docker @buildArgs
    if ($LASTEXITCODE -ne 0) { exit 1 }

    Write-Success "Image built successfully"
}

# =============================================================================
# CLEANUP OLD CONTAINER
# =============================================================================

$containerName = "network-dev"
if (docker ps -aq -f name="^${containerName}$") {
    Write-Info "Removing old container"
    docker rm -f $containerName | Out-Null
}

# =============================================================================
# VOLUMES
# =============================================================================

foreach ($v in @("network-lib-go-cache", "network-lib-ccache")) {
    if (-not (docker volume ls -q -f name="^${v}$")) {
        docker volume create $v | Out-Null
    }
}

# =============================================================================
# START DEV CONTAINER
# =============================================================================

Write-Step "Starting dev container..."

$workspacePath = "/workspaces/network-library"

$dockerArgs = @(
    "run", "-d",
    "--name", $containerName,
    "--init",
    "--cap-add=SYS_PTRACE",
    "--security-opt", "seccomp=unconfined"
)

if ($UseHost) {
    Write-Info "Using host network"
    $dockerArgs += "--network=host"
} else {
    Write-Info "Using port forwarding"
    $dockerArgs += @(
        "-p", "2222:22",        # SSH
        "-p", "8080:8080",
        "-p", "50051:50051",
        "-p", "9090:9090"
    )
}

$dockerArgs += @(
    "-v", "${RepoRoot}:${workspacePath}",
    "-v", "network-lib-go-cache:/go/pkg",
    "-v", "network-lib-ccache:${workspacePath}/.ccache",
    "-w", $workspacePath,
    "-e", "WORKSPACE=${workspacePath}",
    "-e", "CCACHE_DIR=${workspacePath}/.ccache",
    "-e", "CMAKE_EXPORT_COMPILE_COMMANDS=ON",
    "network-lib-dev:latest"
)

& docker @dockerArgs
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Success "Dev container started"

# =============================================================================
# INITIAL SETUP
# =============================================================================

docker exec $containerName bash -c "git config --global --add safe.directory ${workspacePath}" | Out-Null
docker exec $containerName bash -c "mkdir -p build" | Out-Null

# =============================================================================
# USAGE INFO
# =============================================================================

Write-Host "`n══════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  SETUP COMPLETE" -ForegroundColor Green
Write-Host "══════════════════════════════════════════════" -ForegroundColor Cyan

Write-Host "`n📦 VS CODE (Recommended)"
Write-Host "   Dev Containers → Attach to Running Container → network-dev"

Write-Host "`n🔐 SSH ACCESS"
if ($UseHost) {
    Write-Host "   ssh root@localhost"
} else {
    Write-Host "   ssh root@localhost -p 2222"
}
Write-Host "   Password: root"

Write-Host "`n🔧 CLI ACCESS"
Write-Host "   docker exec -it network-dev bash"

Write-Host "`n🛠 BUILD"
Write-Host "   cd build && cmake .. && ninja"

Write-Host "`n══════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "Happy Coding 🚀" -ForegroundColor Green
Write-Host "══════════════════════════════════════════════" -ForegroundColor Cyan

Pop-Location
