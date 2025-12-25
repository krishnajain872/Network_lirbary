Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# -----------------------
# Configuration
# -----------------------
$IMAGE_NAME = "networklib"
$VERSION    = if ($env:VERSION) { $env:VERSION } else { "latest" }
$BUILD_TYPE = if ($env:BUILD_TYPE) { $env:BUILD_TYPE } else { "Release" }
$TARGET     = "runtime"

# -----------------------
# Logging
# -----------------------
function Log-Info {
    param ($Message)
    Write-Host "[INFO]  $Message" -ForegroundColor Green
}

function Log-Error {
    param ($Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

# -----------------------
# Argument Parsing
# -----------------------
for ($i = 0; $i -lt $args.Length; $i++) {
    switch ($args[$i]) {
        "--target" {
            $TARGET = $args[$i + 1]
            $i++
        }
        "--build-type" {
            $BUILD_TYPE = $args[$i + 1]
            $i++
        }
        default {
            Log-Error "Unknown option: $($args[$i])"
            exit 1
        }
    }
}

Log-Info "Building ${IMAGE_NAME}:${VERSION} (target: ${TARGET}, build type: ${BUILD_TYPE})"

# -----------------------
# Docker Build
# -----------------------
$env:DOCKER_BUILDKIT = "1"

docker build `
    --target $TARGET `
    --build-arg BUILD_TYPE=$BUILD_TYPE `
    -t "${IMAGE_NAME}:${VERSION}" `
    -f docker/Dockerfile.build `
    .
