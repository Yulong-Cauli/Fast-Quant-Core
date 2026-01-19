#!/bin/bash
# 构建脚本 - 编译 C++ 核心库

set -e  # 遇到错误立即退出

echo "=================================="
echo "FastQuant 构建脚本"
echo "=================================="

# 检查依赖
echo "检查依赖..."
command -v cmake >/dev/null 2>&1 || { echo "错误: 需要安装 cmake"; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "错误: 需要安装 g++"; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "错误: 需要安装 python3"; exit 1; }

# 检查 pybind11（优先在 venv 中查找，其次在系统中查找）
if ! python3 -c "import pybind11" 2>/dev/null; then
    echo "错误: 未找到 pybind11"
    echo "请确保已激活虚拟环境或已安装 pybind11:"
    echo "  source venv/bin/activate"
    echo "  pip install pybind11[global]"
    exit 1
fi

# 创建构建目录
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "清理旧的构建..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置 CMake
echo "配置 CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON

# 编译
echo "编译中..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . --parallel $NPROC

echo ""
echo "=================================="
echo "构建成功！"
echo "=================================="
echo "C++ 核心库: build/lib/fastquant_cpp*.so"
echo "示例程序: build/bin/example_strategy_test"
echo ""
echo "运行测试: ./build/bin/example_strategy_test"
echo "或者: cd build/bin && ./example_strategy_test"
