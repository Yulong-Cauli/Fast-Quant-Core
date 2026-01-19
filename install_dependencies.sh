#!/bin/bash
# Fast-Quant-Core Dependency Installation Script
# This script installs all required dependencies for building and running Fast-Quant-Core
# Tested on: Ubuntu 24.04 LTS

set -e  # Exit on error

echo "====================================================="
echo "Fast-Quant-Core Dependency Installation"
echo "====================================================="
echo ""

# Check if running on Ubuntu/Debian
if ! command -v apt-get &> /dev/null; then
    echo "Error: This script requires apt-get (Ubuntu/Debian)"
    exit 1
fi

echo "[1/4] Updating package lists..."
sudo apt-get update

echo ""
echo "[2/4] Installing C++ build tools and dependencies..."
# Install essential build tools
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    python3-venv

echo ""
echo "[3/4] Installing pybind11 (Python/C++ binding library)..."
# Install pybind11 with [global] extra for CMake detection
pip3 install --user "pybind11[global]>=2.10.0"

echo ""
echo "[4/4] Installing Python dependencies..."
# Install Python requirements
if [ -f "requirements.txt" ]; then
    pip3 install --user -r requirements.txt
else
    echo "Warning: requirements.txt not found. Skipping Python dependencies."
fi

echo ""
echo "====================================================="
echo "✓ Installation completed successfully!"
echo "====================================================="
echo ""
echo "You can now build the project with:"
echo "  mkdir build && cd build"
echo "  cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON"
echo "  cmake --build . --parallel \$(nproc)"
echo ""
echo "To run example tests:"
echo "  cd build/bin"
echo "  ./example_strategy_test"
echo "  ./test_indicators_optimization"
echo ""
