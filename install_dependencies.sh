#!/bin/bash
# Fast-Quant-Core Dependency Installation Script
# This script installs all required dependencies for building and running Fast-Quant-Core
# Tested on: Ubuntu 24.04 LTS
#
# Usage: chmod +x install_dependencies.sh && ./install_dependencies.sh

set -e  # Exit on error

echo "====================================================="
echo "Fast-Quant-Core Dependency Installation (Ubuntu 24.04)"
echo "====================================================="
echo ""

# Check if running on Ubuntu/Debian
if ! command -v apt-get &> /dev/null; then
    echo "Error: This script requires apt-get (Ubuntu/Debian)"
    exit 1
fi

echo "[1/5] Updating package lists..."
sudo apt-get update

echo ""
echo "[2/5] Installing C++ build tools and dependencies..."
# Install essential build tools
# Note: Skipping GPU/graphics drivers to avoid dpkg conflicts
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    python3-venv

echo ""
echo "[3/5] Creating Python virtual environment..."
# Create venv in the project directory
VENV_DIR="venv"
if [ -d "$VENV_DIR" ]; then
    echo "Virtual environment already exists. Removing old one..."
    rm -rf "$VENV_DIR"
fi
python3 -m venv "$VENV_DIR"
echo "✓ Virtual environment created at ./$VENV_DIR"

echo ""
echo "[4/5] Installing Python dependencies in virtual environment..."
# Activate virtual environment
source "$VENV_DIR/bin/activate"

# Upgrade pip in venv
pip install --upgrade pip

# Install pybind11 with [global] extra for CMake detection
pip install "pybind11[global]>=2.10.0"

# Install Python requirements
if [ -f "requirements.txt" ]; then
    pip install -r requirements.txt
else
    echo "Warning: requirements.txt not found. Skipping Python dependencies."
fi

echo ""
echo "[5/5] Building C++ core library..."
# Run build script with venv activated
if [ -f "scripts/build.sh" ]; then
    ./scripts/build.sh
else
    echo "Warning: scripts/build.sh not found. Skipping build step."
fi

echo ""
echo "====================================================="
echo "✓ Installation completed successfully!"
echo "====================================================="
echo ""
echo "Python virtual environment: ./$VENV_DIR"
echo ""
echo "To activate the virtual environment:"
echo "  source $VENV_DIR/bin/activate"
echo ""
echo "To run example tests:"
echo "  source $VENV_DIR/bin/activate"
echo "  ./build/bin/example_strategy_test"
echo ""
echo "To run Python scripts:"
echo "  source $VENV_DIR/bin/activate"
echo "  python3 scripts/demo.py"
echo ""
echo "Note: GPU/graphics drivers were skipped to avoid dpkg conflicts."
echo ""
