#!/bin/bash
set -e

echo "=========================================="
echo "Starting CI Pipeline"
echo "=========================================="

BUILD_DIR="build-ci"

# 1. Build
echo "[CI] Building Project..."
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON
cmake --build . --parallel $(nproc)
cd ..

# 2. Audit (Unit + Integration)
echo "[CI] Running Audit..."
python3 scripts/audit/run_audit.py

# 3. Benchmark
echo "[CI] Running Benchmark..."
# Requires pyyaml
pip install pyyaml || true
python3 scripts/benchmark/run_load_test.py

echo "=========================================="
echo "CI Pipeline Completed Successfully"
echo "=========================================="
