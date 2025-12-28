#!/usr/bin/env bash
set -e

# ==============================
# 🎨 Colors
# ==============================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# ==============================
# 🧱 Defaults
# ==============================
TARGET="all"
BUILD_TYPE="Debug"
CLEAN_BUILD=true

# ==============================
# 🔍 Parse arguments
# ==============================
for arg in "$@"; do
    case $arg in
        --target=*)
            TARGET="${arg#*=}"
            ;;
        --type=*)
            BUILD_TYPE="${arg#*=}"
            ;;
        --no-clean)
            CLEAN_BUILD=false
            ;;
        --help|-h)
            echo -e "${CYAN}Usage:${NC}"
            echo "  ./build.sh [--target=all|client|server|logger|protos]"
            echo "             [--type=Debug|Release]"
            echo "             [--no-clean]"
            exit 0
            ;;
        *)
            echo -e "${YELLOW}⚠️  Unknown option ignored: $arg${NC}"
            ;;
    esac
done

# ==============================
# 📌 Header
# ==============================
echo -e "${CYAN}"
echo "=============================="
echo " Network Library Build Script "
echo "=============================="
echo -e "${NC}"

echo -e "🔧 Build Type : ${BLUE}${BUILD_TYPE}${NC}"
echo -e "🎯 Target     : ${BLUE}${TARGET}${NC}"
echo -e "🧹 Clean Build: ${BLUE}${CLEAN_BUILD}${NC}"

# ==============================
# 📁 Build directory
# ==============================
BUILD_DIR="build-${BUILD_TYPE}"

if [ "$CLEAN_BUILD" = true ]; then
    if [ -d "$BUILD_DIR" ]; then
        echo -e "\n${YELLOW}🧹 Cleaning old build directory...${NC}"
        rm -rf "$BUILD_DIR"
    fi

    if [ -d "gen" ]; then
        echo -e "${YELLOW}🧹 Removing generated protobuf files (gen/) ...${NC}"
        rm -rf gen
    fi
fi

echo -e "\n${CYAN}📁 Creating build directory: ${BUILD_DIR}${NC}"
mkdir -p "$BUILD_DIR"

# ==============================
# 🎯 Map targets
# ==============================
case "$TARGET" in
    client)
        CMAKE_TARGET="oms_client"
        ;;
    server)
        CMAKE_TARGET="oms_server"
        ;;
    logger)
        CMAKE_TARGET="logger"
        ;;
    protos)
        CMAKE_TARGET="protos"
        ;;
    all)
        CMAKE_TARGET="all"
        ;;
    *)
        echo -e "${RED}❌ Unknown target: ${TARGET}${NC}"
        exit 1
        ;;
esac

# ==============================
# ⚙️ Configure
# ==============================
echo -e "\n${CYAN}⚙️  Configuring CMake...${NC}"
cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

# ==============================
# 🚀 Build
# ==============================
echo -e "\n${CYAN}🚀 Building (${CMAKE_TARGET})...${NC}"
if [ "$CMAKE_TARGET" = "all" ]; then
    cmake --build "$BUILD_DIR" --parallel "$(nproc)"
else
    cmake --build "$BUILD_DIR" \
        --target "$CMAKE_TARGET" \
        --parallel "$(nproc)"
fi

# ==============================
# ✅ Done
# ==============================
echo -e "\n${GREEN}✅ Build completed successfully!${NC}"
echo -e "📂 Binaries : ${BLUE}${BUILD_DIR}/bin${NC}"
echo -e "📂 Libraries: ${BLUE}${BUILD_DIR}/lib${NC}"
