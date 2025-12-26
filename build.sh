#!/usr/bin/env bash
set -e

echo "=============================="
echo " Network Library Build Script "
echo "=============================="
echo ""
echo "Select build mode:"
echo "1) Debug"
echo "2) Release"
echo "3) RelWithDebInfo"
echo "4) MinSizeRel"
echo "5) ASan"
echo ""

read -p "Enter choice [1-5]: " choice

case $choice in
  1) BUILD_TYPE="Debug" ;;
  2) BUILD_TYPE="Release" ;;
  3) BUILD_TYPE="RelWithDebInfo" ;;
  4) BUILD_TYPE="MinSizeRel" ;;
  5) BUILD_TYPE="ASan" ;;
  *)
    echo "❌ Invalid choice"
    exit 1
    ;;
esac

BUILD_DIR="build-${BUILD_TYPE}"

echo ""
echo "🔧 Build Type: ${BUILD_TYPE}"
echo "📁 Build Dir : ${BUILD_DIR}"
echo ""

# 🔥 Clean existing build directory
if [ -d "${BUILD_DIR}" ]; then
  echo "🧹 Removing existing build directory..."
  rm -rf "${BUILD_DIR}"
fi

# 📁 Create fresh build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo "⚙️ Configuring CMake..."
cmake .. \
  -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

echo ""
echo "🚀 Building..."
cmake --build . --parallel $(nproc)

echo ""
echo "📦 Installing..."
cmake --install .

echo ""
echo "✅ Clean build completed successfully!"
