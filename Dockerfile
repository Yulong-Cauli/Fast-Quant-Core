# ==================== 开发阶段 ====================
FROM ubuntu:22.04 AS builder

# 安装编译依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 复制项目文件
COPY . .

# 安装 Python 依赖
RUN pip3 install --no-cache-dir pybind11[global]

# 编译 C++ 核心库
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON && \
    cmake --build . --parallel $(nproc)

# ==================== 运行阶段 ====================
FROM ubuntu:22.04

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# 创建应用目录
WORKDIR /app

# 从构建阶段复制编译产物
COPY --from=builder /app/build/lib/*.so /app/app/
COPY --from=builder /app/app /app/app
COPY --from=builder /app/config /app/config

# 安装 Python 运行时依赖
COPY requirements.txt .
RUN pip3 install --no-cache-dir -r requirements.txt

# 创建数据和日志目录
RUN mkdir -p /app/data /app/logs

# 设置环境变量
ENV PYTHONPATH=/app
ENV PYTHONUNBUFFERED=1

# 暴露端口（如果需要 Web 界面）
# EXPOSE 8000

# 入口命令
CMD ["python3", "-m", "app.main"]
