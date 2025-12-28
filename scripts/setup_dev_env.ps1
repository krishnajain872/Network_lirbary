# scripts/setup_dev_env.ps1
# Sets up the Windows development environment for Network Library

#Requires -Version 5.1

[CmdletBinding()]
param(
    [switch]$SkipImageBuild,
    [switch]$NoBuildCache
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

function Write-Detail {
    param([string]$Message)
    if ($PSCmdlet.MyInvocation.BoundParameters["Verbose"].IsPresent) {
        Write-Verbose $Message
    }
}

# =============================================================================
# ENVIRONMENT CHECKS
# =============================================================================

Write-Step "Checking prerequisites..."

# 1. Check for Docker
if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Error "Docker is not installed or not in your PATH. Please install Docker Desktop for Windows."
    exit 1
}

# Check if Docker daemon is running
try {
    docker ps 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        throw "Docker daemon not responding"
    }
    Write-Success "Docker is running"
    Write-Detail "Docker version: $(docker --version)"
} catch {
    Write-Error "Docker daemon is not running. Please start Docker Desktop."
    exit 1
}

# Check Docker BuildKit support
$env:DOCKER_BUILDKIT = "1"
$env:COMPOSE_DOCKER_CLI_BUILD = "1"
Write-Success "BuildKit enabled for faster builds"
Write-Detail "DOCKER_BUILDKIT=$env:DOCKER_BUILDKIT"

# 2. Move to repository root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path "$ScriptDir\.."
Set-Location $RepoRoot
Write-Success "Repository root: $RepoRoot"
Write-Detail "Working directory: $(Get-Location)"

# 3. Verify Dockerfile exists
$DockerfilePath = Join-Path $RepoRoot ".devcontainer\Dockerfile"
if (-not (Test-Path $DockerfilePath)) {
    Write-Error "Dockerfile not found at: $DockerfilePath"
    exit 1
}
Write-Success "Dockerfile found"
Write-Detail "Dockerfile path: $DockerfilePath"

# =============================================================================
# IMAGE BUILD
# =============================================================================

if (-not $SkipImageBuild) {
    Write-Step "Building dev container image..."
    Write-Info "This may take 5-15 minutes on first build"
    Write-Info "Subsequent builds will be much faster (1-2 minutes)"

    $buildArgs = @(
        "build",
        "-f", ".devcontainer/Dockerfile",
        "-t", "network-lib-dev:latest"
    )

    # Add progress flag based on verbose mode
    if ($PSCmdlet.MyInvocation.BoundParameters["Verbose"].IsPresent) {
        $buildArgs += "--progress=plain"
        Write-Detail "Using plain progress output (verbose mode)"
    } else {
        $buildArgs += "--progress=auto"
    }

    if (-not $NoBuildCache) {
        Write-Info "Using build cache (use -NoBuildCache to force clean build)"
        Write-Detail "Cache will be used from previous builds"
    } else {
        $buildArgs += "--no-cache"
        Write-Info "Clean build requested (no cache)"
        Write-Detail "All layers will be rebuilt from scratch"
    }

    $buildArgs += "."

    Write-Detail "Build command: docker $($buildArgs -join ' ')"
    Write-Detail "Build context: $(Get-Location)"
    
    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    
    try {
        Write-Detail "Starting Docker build process..."
        
        if ($PSCmdlet.MyInvocation.BoundParameters["Verbose"].IsPresent) {
            # In verbose mode, show all output
            & docker @buildArgs
        } else {
            # In normal mode, show summary only
            $output = & docker @buildArgs 2>&1
            if ($LASTEXITCODE -ne 0) {
                # On error, show last 20 lines
                Write-Host ($output | Select-Object -Last 20 | Out-String)
            }
        }
        
        if ($LASTEXITCODE -ne 0) {
            throw "Docker build failed with exit code $LASTEXITCODE"
        }
        
        $stopwatch.Stop()
        Write-Success "Image built successfully in $($stopwatch.Elapsed.TotalSeconds.ToString('F1'))s"
        
        # Show image details
        $imageInfo = docker images network-lib-dev:latest --format "{{.Size}}" 2>$null
        if ($imageInfo) {
            Write-Detail "Image size: $imageInfo"
        }
        
    } catch {
        Write-Error "Failed to build Docker image: $_"
        Write-Host "`nTroubleshooting tips:" -ForegroundColor Yellow
        Write-Host "  1. Ensure Docker Desktop is running"
        Write-Host "  2. Check if you have enough disk space"
        Write-Host "  3. Try running with -Verbose flag for detailed output"
        Write-Host "  4. Try running with -NoBuildCache for clean build"
        exit 1
    }
} else {
    Write-Step "Skipping image build (using existing image)"
    
    # Verify image exists
    $imageExists = docker images -q network-lib-dev:latest 2>$null
    if (-not $imageExists) {
        Write-Error "Image 'network-lib-dev:latest' not found. Remove -SkipImageBuild flag."
        exit 1
    }
    Write-Success "Using existing image"
    
    # Show image details
    $imageInfo = docker images network-lib-dev:latest --format "table {{.Repository}}\t{{.Tag}}\t{{.Size}}\t{{.CreatedAt}}" 2>$null
    if ($imageInfo -and $PSCmdlet.MyInvocation.BoundParameters["Verbose"].IsPresent) {
        Write-Detail "`n$imageInfo"
    }
}

# =============================================================================
# CLEANUP OLD CONTAINERS
# =============================================================================

Write-Step "Checking for existing containers..."

$existingContainer = docker ps -aq -f name=network-dev 2>$null
if ($existingContainer) {
    Write-Info "Removing old container..."
    Write-Detail "Container ID: $existingContainer"
    docker rm -f network-dev 2>&1 | Out-Null
    Write-Success "Old container removed"
} else {
    Write-Info "No existing containers found"
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
Write-Host "Dev Containers: Reopen in Container" -ForegroundColor White
Write-Host "   4. Wait for container initialization (~30s)"

Write-Host "`n🐳 OPTION 2: Run Container Manually" -ForegroundColor Yellow
Write-Host @"
   Start container:
   docker run -d --name network-dev \
     -p 2222:22 \
     --cap-add=SYS_PTRACE \
     --security-opt seccomp=unconfined \
     -v "${RepoRoot}:/workspaces/network-library" \
     network-lib-dev:latest

   Stop container:
   docker stop network-dev

   Remove container:
   docker rm network-dev
"@

Write-Host "`n🔧 OPTION 3: Interactive Shell" -ForegroundColor Yellow
Write-Host @"
   docker run -it --rm \
     -v "${RepoRoot}:/workspaces/network-library" \
     -w /workspaces/network-library \
     network-lib-dev:latest bash
"@

Write-Host "`n📝 QUICK COMMANDS" -ForegroundColor Yellow
Write-Host "   Rebuild image:      " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1" -ForegroundColor White
Write-Host "   Clean rebuild:      " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -NoBuildCache" -ForegroundColor White
Write-Host "   Skip rebuild:       " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -SkipImageBuild" -ForegroundColor White
Write-Host "   Verbose output:     " -NoNewline
Write-Host ".\scripts\setup_dev_env.ps1 -Verbose" -ForegroundColor White
Write-Host "   View image size:    " -NoNewline
Write-Host "docker images network-lib-dev" -ForegroundColor White
Write-Host "   Remove image:       " -NoNewline
Write-Host "docker rmi network-lib-dev:latest" -ForegroundColor White

Write-Host "`n═══════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Happy Coding! 🚀" -ForegroundColor Green
Write-Host "═══════════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

# Return to original directory
Pop-Location -ErrorAction SilentlyContinue

Write-Detail "Script completed successfully"