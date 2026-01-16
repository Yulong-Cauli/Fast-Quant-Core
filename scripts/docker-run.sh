#!/bin/bash
# Docker 启动脚本

echo "=================================="
echo "FastQuant Docker 部署"
echo "=================================="

# 检查 Docker
command -v docker >/dev/null 2>&1 || { echo "错误: 需要安装 Docker"; exit 1; }
command -v docker-compose >/dev/null 2>&1 || { echo "错误: 需要安装 docker-compose"; exit 1; }

# 检查配置文件
if [ ! -f "config/config.yaml" ]; then
    echo "警告: 配置文件不存在，将使用环境变量"
fi

# 构建镜像
echo "构建 Docker 镜像..."
docker-compose build

# 启动容器
echo "启动容器..."
docker-compose up -d

echo ""
echo "=================================="
echo "部署完成！"
echo "=================================="
echo "查看日志: docker-compose logs -f"
echo "停止服务: docker-compose down"
