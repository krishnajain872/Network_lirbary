#!/usr/bin/env bash
set -e
BUILD_TYPE="Debug"
BUILD_DIR="build-${BUILD_TYPE}"
if [ -d "${BUILD_DIR}" ]; then
  rm -rf "${BUILD_DIR}"
fi
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build . --parallel $(nproc)
ctest --output-on-failure
