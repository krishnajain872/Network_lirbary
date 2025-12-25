#!/bin/bash

# setup_debug.sh
# Sets up the environment for debugging

echo "Setting up VSCode Debug Environment..."

# 1. Install recommended extensions
echo "Recommended VSCode Extensions:"
echo "- ms-vscode.cpptools (C/C++)"
echo "- ms-vscode.cmake-tools (CMake Tools)"
echo "- twxs.cmake (CMake Language Support)"

# 2. Configure CMake for Debug
echo "Configuring CMake for Debug build..."
mkdir -p ../build
cd ../
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "Setup complete. Open the folder in VSCode and press F5 to debug."
