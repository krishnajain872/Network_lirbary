#!/usr/bin/env bash
set -e

echo "=============================="
echo " Network Library Build Script "
echo "=============================="

TARGET="all"
BUILD_TYPE="Debug"

# Parse arguments
for i in "$@"
do
case $i in
    --target=*)
    TARGET="${i#*=}"
    shift
    ;;
    --type=*)
    BUILD_TYPE="${i#*=}"
    shift
    ;;
    *)
    # unknown option
    ;;
esac
done

echo "🔧 Build Type: ${BUILD_TYPE}"
echo "🎯 Target    : ${TARGET}"

BUILD_DIR="build-${BUILD_TYPE}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Map target names to CMake targets
CMAKE_TARGET=""
if [ "$TARGET" == "client" ]; then
    CMAKE_TARGET="oms_client"
elif [ "$TARGET" == "server" ]; then
    CMAKE_TARGET="oms_server"
elif [ "$TARGET" == "logger" ]; then
    CMAKE_TARGET="logger"
elif [ "$TARGET" == "protos" ]; then
    CMAKE_TARGET="protos"
elif [ "$TARGET" == "all" ]; then
    CMAKE_TARGET="all"
else
    echo "❌ Unknown target: $TARGET"
    exit 1
fi

echo "⚙️ Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

echo "🚀 Building..."
if [ "$CMAKE_TARGET" == "all" ]; then
    cmake --build . --parallel $(nproc)
else
    cmake --build . --target $CMAKE_TARGET --parallel $(nproc)
fi

echo ""
echo "✅ Build completed successfully!"
echo "📂 Binaries: ${BUILD_DIR}/bin"
echo "📂 Libraries: ${BUILD_DIR}/lib"
