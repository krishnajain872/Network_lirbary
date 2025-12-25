#!/bin/bash
# scripts/docker_build.sh

set -euo pipefail

# Configuration
IMAGE_NAME="networklib"
VERSION="${VERSION:-latest}"
BUILD_TYPE="${BUILD_TYPE:-Release}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse arguments
TARGET="runtime"

while [[ $# -gt 0 ]]; do
    case $1 in
        --target)
            TARGET="$2"
            shift 2
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

log_info "Building ${IMAGE_NAME}:${VERSION} (target: ${TARGET}, build type: ${BUILD_TYPE})"

# Build arguments
export DOCKER_BUILDKIT=1

docker build \
    --target ${TARGET} \
    --build-arg BUILD_TYPE=${BUILD_TYPE} \
    -t ${IMAGE_NAME}:${VERSION} \
    -f docker/Dockerfile.build \
    .
