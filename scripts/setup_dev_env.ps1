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
    Write-Error "Docker is not installed or not in your PATH. Please install Docker Desktop for Windows."
    exit 1
}

try {
    docker ps 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) { throw }
    Write-Success "Docker is running"
} catch {
    Write-Error "Docker daemon is not running. Please start Docker Desktop."
    exit 1
}

# Check Docker version
$dockerVersion = docker version --format '{{.Server.Version}}' 2>$null
Write-Info "Docker version: $dockerVersion"

$env:DOCKER_BUILDKIT = "1"
$env:COMPOSE_DOCKER_CLI_BUILD = "1"
Write-Success "BuildKit enabled for faster builds"

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
    Write-Step "Start-only mode enabled (no build)"

    $imageExists = docker images -q network-lib-dev:latest 2>$null
    if (-not $imageExists) {
        Write-Error "Image 'network-lib-dev:latest' not found. Build it first without -StartOnly flag."
        exit 1
    }

    Write-Success "Existing image found"
    $SkipImageBuild = $true
}

# =============================================================================
# DOCKERFILE CHECK
# =============================================================================

if (-not $StartOnly) {
    $DockerfilePath = Join-Path $RepoRoot ".devcontainer\Dockerfile"
    if (-not (Test-Path $DockerfilePath)) {
        Write-Error "Dockerfile not found at: $DockerfilePath"
        exit 1
    }
    Write-Success "Dockerfile found"
}

# =============================================================================
# IMAGE BUILD
# =============================================================================

if (-not $SkipImageBuild) {
    Write-Step "Building dev container image..."
    Write-Info "This may take 10-15 minutes on first build..."

    $buildArgs = @(
        "build",
        "-f", ".devcontainer/Dockerfile",
        "-t", "network-lib-dev:latest",
        "--build-arg", "BUILDKIT_INLINE_CACHE=1",
        "--build-arg", "DOCKER_BUILDKIT=1"
    )

    if ($PSCmdlet.MyInvocation.BoundParameters["Verbose"].IsPresent) {
        $buildArgs += "--progress=plain"
    } else {
        $buildArgs += "--progress=auto"
    }

    if ($NoBuildCache) {
        $buildArgs += "--no-cache"
        Write-Warning-Custom "Clean build requested - this will take longer"
    }

    $buildArgs += "."

    try {
        $buildStart = Get-Date
        & docker @buildArgs
        if ($LASTEXITCODE -ne 0) { throw }
        $buildEnd = Get-Date
        $buildTime = ($buildEnd - $buildStart).TotalSeconds
        Write-Success "Image built successfully in $([math]::Round($buildTime, 1)) seconds"
        
        # Show image size
        $imageSize = docker images network-lib-dev:latest --format "{{.Size}}" 2>$null
        Write-Info "Image size: $imageSize"
    } catch {
        Write-Error "Docker build failed. Try running with -Verbose for more details."
        exit 1
    }
} else {
    Write-Step "Skipping image build"
}

# =============================================================================
# CLEANUP OLD CONTAINER
# =============================================================================

Write-Step "Checking for existing containers..."

$containerName = "network-dev"
$existingContainer = docker ps -aq -f name="^${containerName}$" 2>$null

if ($existingContainer) {
    Write-Info "Removing old container..."
    docker rm -f $containerName 2>&1 | Out-Null
    Write-Success "Old container removed"
} else {
    Write-Info "No existing containers found"
}

# =============================================================================
# CREATE PERSISTENT VOLUMES
# =============================================================================

Write-Step "Setting up persistent volumes..."

$volumes = @(
    "network-lib-go-cache",
    "network-lib-ccache"
)

foreach ($volume in $volumes) {
    $volumeExists = docker volume ls -q -f name="^${volume}$" 2>$null
    if (-not $volumeExists) {
        docker volume create $volume | Out-Null
        Write-Info "Created volume: $volume"
    } else {
        Write-Info "Volume exists: $volume"
    }
}

Write-Success "Volumes ready"

# =============================================================================
# START DEV CONTAINER
# =============================================================================

Write-Step "Starting dev container..."

$workspacePath = "/workspaces/network-library"

# Base docker run arguments
$dockerArgs = @(
    "run", "-d",
    "--name", $containerName,
    "--init",
    "--cap-add=SYS_PTRACE",
    "--security-opt", "seccomp=unconfined"
)

# Network configuration
if ($UseHost) {
    Write-Info "Using host network mode"
    $dockerArgs += "--network=host"
} else {
    Write-Info "Using port forwarding mode"
    $dockerArgs += @(
        "-p", "8080:8080",
        "-p", "50051:50051",
        "-p", "9090:9090"
    )
}

# Volume mounts
$dockerArgs += @(
    "-v", "${RepoRoot}:${workspacePath}",
    "-v", "network-lib-go-cache:/go/pkg",
    "-v", "network-lib-ccache:${workspacePath}/.ccache",
    "-w", $workspacePath
)

# Environment variables
$dockerArgs += @(
    "-e", "WORKSPACE=${workspacePath}",
    "-e", "CCACHE_DIR=${workspacePath}/.ccache",
    "-e", "CMAKE_EXPORT_COMPILE_COMMANDS=ON"
)

# Image name
$dockerArgs += "network-lib-dev:latest"

try {
    & docker @dockerArgs | Out-Null

    if ($LASTEXITCODE -ne 0) { throw }

    # Wait for container to be ready (max 10 seconds)
    $timeout = 10
    $elapsed = 0
    do {
        Start-Sleep -Milliseconds 500
        $elapsed += 0.5
        $containerState = docker inspect -f '{{.State.Running}}' $containerName 2>$null
        if ($containerState -eq "true") { break }
    } while ($elapsed -lt $timeout)

    if ($containerState -ne "true") {
        Write-Error "Container failed to start within ${timeout}s"
        docker logs $containerName
        exit 1
    }

    Write-Success "Dev container started successfully"
    Write-Info "Container name : $containerName"
    Write-Info "Workspace      : $workspacePath"
    
    if (-not $UseHost) {
        Write-Info "HTTP Port      : 8080"
        Write-Info "gRPC Port      : 50051"
        Write-Info "Metrics Port   : 9090"
    }

    # Run initial setup commands
    Write-Step "Running initial setup..."
    docker exec $containerName bash -c "git config --global --add safe.directory ${workspacePath}" 2>&1 | Out-Null
    docker exec $containerName bash -c "mkdir -p build" 2>&1 | Out-Null
    Write-Success "Initial setup complete"

} catch {
    Write-Error "Failed to start dev container"
    Write-Info "Checking logs..."
    docker logs $containerName 2>&1
    exit 1
}

# =============================================================================
# VERIFY CONTAINER HEALTH
# =============================================================================

Write-Step "Verifying container health..."

try {
    $testResult = docker exec $containerName bash -c "echo OK" 2>$null
    if ($testResult -eq "OK") {
        Write-Success "Container is healthy and responsive"
    } else {
        Write-Warning-Custom "Container may not be fully ready"
    }
} catch {
    Write-Warning-Custom "Could not verify container health"
}

# =============================================================================
# USAGE INSTRUCTIONS
# =============================================================================

Write-Host "`n" -NoNewline
Write-Host "═══════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  SETUP COMPLETE!" -ForegroundColor Green
Write-Host "═══════════════════════════════════════════════════════════════" -ForegroundColor Cyan

Write-Host "`n📦 OPTION 1: VS Code Dev Containers (Recommended)" -ForegroundColor Yellow
Write-Host "   1. Open this folder in VS Code"
Write-Host "   2. Press " -NoNewline
Write-Host "F1" -ForegroundColor White -NoNewline
Write-Host " or " -NoNewline
Write-Host "Ctrl+Shift+P" -ForegroundColor White
Write-Host "   3. Select: " -NoNewline
Write-Host "Dev Containers: Attach to Running Container" -ForegroundColor White
Write-Host "   4. Choose: " -NoNewline
Write-Host "network-dev" -ForegroundColor White

Write-Host "`n🔧 OPTION 2: Command Line Access" -ForegroundColor Yellow
Write-Host "   Access container shell:"
Write-Host "   " -NoNewline
Write-Host "docker exec -it network-dev bash" -ForegroundColor White

Write-Host "`n   Build your project:"
Write-Host "   " -NoNewline
Write-Host "docker exec -it network-dev bash -c 'cd build && cmake .. && ninja'" -ForegroundColor White

Write-Host "`n🐳 CONTAINER MANAGEMENT" -ForegroundColor Yellow
Write-Host "   View logs:          " -NoNewline
Write-Host "docker logs network-dev" -ForegroundColor White
Write-Host "   Stop container:     " -NoNewline
Write-Host "docker stop network-dev" -ForegroundColor White
Write-Host "   Start container:    " -NoNewline
Write-Host "docker start network-dev" -ForegroundColor White
Write-Host "   Remove container:   " -NoNewline
Write-Host "docker rm -f network-dev" -ForegroundColor White
Write-Host "   Container status:   " -NoNewline
Write-Host "docker ps -a -f name=network-dev" -ForegroundColor White

Write-Host "`n📝 SCRIPT OPTIONS" -ForegroundColor Yellow
Write-Host "   Rebuild image:      " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1" -ForegroundColor White
Write-Host "   Clean rebuild:      " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -NoBuildCache" -ForegroundColor White
Write-Host "   Skip rebuild:       " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -SkipImageBuild" -ForegroundColor White
Write-Host "   Start only:         " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -StartOnly" -ForegroundColor White
Write-Host "   Use host network:   " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -UseHost" -ForegroundColor White
Write-Host "   Verbose output:     " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -Verbose" -ForegroundColor White

Write-Host "`n🧹 CLEANUP COMMANDS" -ForegroundColor Yellow
Write-Host "   Remove all:         " -NoNewline
Write-Host "docker rm -f network-dev; docker rmi network-lib-dev:latest" -ForegroundColor White
Write-Host "   Clean volumes:      " -NoNewline
Write-Host "docker volume rm network-lib-go-cache network-lib-ccache" -ForegroundColor White
Write-Host "   View image size:    " -NoNewline
Write-Host "docker images network-lib-dev" -ForegroundColor White

Write-Host "`n🔍 TROUBLESHOOTING" -ForegroundColor Yellow
Write-Host "   Container won't start? Check logs with: " -NoNewline
Write-Host "docker logs network-dev" -ForegroundColor White
Write-Host "   Build failing? Try clean build: " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -NoBuildCache" -ForegroundColor White
Write-Host "   Slow builds? Ensure Docker BuildKit is enabled (automatic in this script)" -ForegroundColor DarkGray

Write-Host "`n═══════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Happy Coding! 🚀" -ForegroundColor Green
Write-Host "═══════════════════════════════════════════════════════════════" -ForegroundColor Cyan

Pop-Location