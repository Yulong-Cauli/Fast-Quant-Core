# 快速测试指南

## 0. 安装依赖（首次运行）

如果您还没有安装依赖，请先运行：

```bash
chmod +x install_dependencies.sh
./install_dependencies.sh
```

这将自动安装所有依赖并编译 C++ 核心库。

**Ubuntu 24.04+ 用户**：脚本会创建虚拟环境（venv）以符合系统规范。

## 1. 编译 C++ 核心库

```bash
# 激活虚拟环境（如果尚未激活）
source venv/bin/activate

# 方法 1: 使用构建脚本（推荐）
./scripts/build.sh

# 方法 2: 手动编译
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON
cmake --build . --parallel $(nproc)
```

## 2. 测试 C++ 核心功能

```bash
# 运行示例测试程序
./build/bin/example_strategy_test
```

预期输出：
```
=== FastQuant 策略测试 ===
...
处理 10,000 个数据点，计算 SMA(20)
耗时: 76 μs
...
```

## 3. 测试 Python 绑定

```bash
# 激活虚拟环境（如果尚未激活）
source venv/bin/activate

# 运行演示脚本（无需 Binance API）
python3 scripts/demo.py
```

预期输出：
```
FastQuant 核心功能演示
1️⃣  技术指标计算
...
✅ 演示完成！
```

## 4. 运行完整系统（需要 Binance API）

```bash
# 激活虚拟环境
source venv/bin/activate

# 1. 配置 API 密钥
cp config/config.example.yaml config/config.yaml
vim config/config.yaml  # 填入你的 API 密钥

# 2. 启动交易机器人
./scripts/run.sh
```

## 常见问题

### Q: 编译时找不到 pybind11？

确保已激活虚拟环境：
```bash
source venv/bin/activate
# 如果仍然缺少 pybind11
pip install "pybind11[global]"
```

### Q: Ubuntu 24.04 安装时出现 dpkg 错误？

使用新版 `install_dependencies.sh`，它会跳过 GPU 驱动安装，专注于 C++ 编译环境。

### Q: 如何只测试不连接交易所？

使用 `scripts/demo.py`，它不需要 Binance API，只演示 C++ 核心功能。

### Q: 如何使用测试网？

在 `config/config.yaml` 中设置：
```yaml
binance:
  testnet: true
  enable_trading: false  # 先用模拟模式
```

## 项目结构速览

```
核心文件：
- core/include/*.h         # C++ 核心库（Header-only）
- bindings/bindings_main.cpp  # pybind11 绑定
- app/*.py                 # Python 应用层
- tests/example_strategy_test.cpp  # C++ 测试
- scripts/demo.py          # Python 演示（无需API）
```

## 面试展示建议

1. **从 C++ 测试开始**：展示算法和 STL 能力
   ```bash
   ./build/bin/example_strategy_test
   ```

2. **展示 Python 集成**：展示混合编程
   ```bash
   python3 scripts/demo.py
   ```

3. **讲解架构设计**：打开 `ARCHITECTURE.md`

4. **展示 CI/CD**：展示 `.github/workflows/build.yml`

5. **讲解 Docker 部署**：展示 `Dockerfile` 和 `docker-compose.yml`
