#!/bin/bash
# 快速启动脚本

set -e

echo "=================================="
echo "FastQuant 快速启动"
echo "=================================="

# 检查并激活虚拟环境
if [ -d "venv" ] && [ -f "venv/bin/activate" ]; then
    echo "激活虚拟环境..."
    source venv/bin/activate
else
    echo "警告: 未找到虚拟环境 (venv)"
    echo "建议运行: ./install_dependencies.sh"
fi

# 检查配置文件
if [ ! -f "config/config.yaml" ]; then
    echo "错误: 配置文件不存在"
    echo "请先创建配置文件: cp config/config.example.yaml config/config.yaml"
    echo "然后编辑配置文件，填入你的 API 密钥"
    exit 1
fi

# 检查 C++ 模块
if [ ! -f "app/fastquant_cpp"*.so ] && [ ! -f "build/lib/fastquant_cpp"*.so ]; then
    echo "C++ 模块未编译，正在编译..."
    ./scripts/build.sh
    
    # 复制编译产物
    if [ -f build/lib/fastquant_cpp*.so ]; then
        cp build/lib/fastquant_cpp*.so app/
    fi
fi

# 检查 Python 依赖
echo "检查 Python 依赖..."
pip install -q -r requirements.txt

# 启动程序
echo ""
echo "启动交易机器人..."
echo "=================================="
python3 -m app.main
