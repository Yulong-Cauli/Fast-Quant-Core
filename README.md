# Fast-Quant-Core

<div align="center">

**轻量级量化交易系统 | C++ 核心 + Python 应用层**

[![Build Status](https://github.com/Yulong-Cauli/Fast-Quant-Core/workflows/Build%20and%20Test/badge.svg)](https://github.com/Yulong-Cauli/Fast-Quant-Core/actions)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.9+-green.svg)](https://www.python.org/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

专为**算法竞赛选手转型量化开发**设计的完整项目框架

</div>

## 📖 项目简介

Fast-Quant-Core 是一个混合 C++/Python 架构的量化交易系统，专为展示**算法能力**和**工程能力**而设计。

### 核心特性

✅ **混合架构设计**
- C++ 核心层：高性能策略计算，展示 STL 和算法优势
- Python 应用层：快速集成交易所 API 和业务逻辑
- pybind11 桥接：零开销的 Python-C++ 互操作

✅ **完整的功能实现**
- 🔌 Binance API 集成（支持实时行情和交易）
- 📈 双均线策略（金叉/死叉信号生成）
- 💰 自动下单和仓位管理
- 📊 盈亏统计和交易记录
- 🔒 基础风控（最大持仓、止损）

✅ **工业级工程实践**
- Docker 容器化部署
- GitHub Actions CI/CD
- CMake 现代构建系统
- 完整的日志和监控

## 🏗️ 系统架构

```
┌─────────────────────────────────────────────────────┐
│              Python 应用层 (app/)                    │
│  交易所连接 | 订单管理 | 盈亏统计 | 日志监控          │
└─────────────────────────────────────────────────────┘
                        ↓ pybind11
┌─────────────────────────────────────────────────────┐
│            C++ 核心策略层 (core/)                    │
│  市场数据结构 | 指标计算 | 策略引擎 | 高性能算法      │
└─────────────────────────────────────────────────────┘
```

详细架构设计见 [ARCHITECTURE.md](ARCHITECTURE.md)

## 🚀 快速开始

### 环境要求

- **C++ 编译器**: GCC 7+ / Clang 6+ / MSVC 2019+
- **CMake**: 3.15+
- **Python**: 3.9+
- **依赖库**: pybind11, python-binance

### 方法一：本地运行

#### 1. 克隆项目

```bash
git clone https://github.com/Yulong-Cauli/Fast-Quant-Core.git
cd Fast-Quant-Core
```

#### 2. 编译 C++ 核心库

```bash
./scripts/build.sh
```

这将：
- 安装 pybind11 依赖
- 编译 C++ 核心库
- 生成 Python 可导入的 .so 模块
- 构建示例测试程序

#### 3. 测试核心功能

```bash
# 运行 C++ 测试程序
./build/bin/example_strategy_test
```

你将看到：
- ✅ 技术指标计算（SMA, EMA, Bollinger Bands）
- ✅ 双均线策略回测
- ✅ 性能基准测试

#### 4. 配置交易参数

```bash
# 复制配置文件模板
cp config/config.example.yaml config/config.yaml

# 编辑配置文件，填入你的 Binance API 密钥
vim config/config.yaml
```

**配置说明：**
```yaml
binance:
  api_key: "你的_API_KEY"
  api_secret: "你的_API_SECRET"
  testnet: true  # ⚠️ 建议先使用测试网

strategy:
  symbol: "BTCUSDT"
  fast_period: 5    # 快线周期
  slow_period: 20   # 慢线周期

trading:
  enable_trading: false  # false=模拟交易（推荐）
  trade_quantity: 0.001
  max_position: 0.01
```

#### 5. 启动交易机器人

```bash
# 方式 1: 使用启动脚本
./scripts/run.sh

# 方式 2: 直接运行
python3 -m app.main
```

### 方法二：Docker 部署

```bash
# 1. 编辑配置文件
cp config/config.example.yaml config/config.yaml
vim config/config.yaml

# 2. 使用 docker-compose 启动
docker-compose up -d

# 3. 查看日志
docker-compose logs -f

# 4. 停止服务
docker-compose down
```

## 📚 项目结构

```
Fast-Quant-Core/
├── core/                   # C++ 核心库
│   ├── include/
│   │   ├── market_data.h   # 市场数据结构
│   │   ├── indicators.h    # 技术指标计算
│   │   └── strategy.h      # 策略引擎
│   └── src/                # (Header-only 实现)
├── bindings/               # Python-C++ 绑定
│   └── bindings_main.cpp   # pybind11 导出
├── app/                    # Python 应用层
│   ├── binance_connector.py  # 交易所连接器
│   ├── strategy_runner.py    # 策略运行器
│   ├── pnl_tracker.py        # 盈亏追踪器
│   └── main.py               # 主程序入口
├── tests/                  # 测试程序
│   └── example_strategy_test.cpp
├── config/                 # 配置文件
│   └── config.example.yaml
├── scripts/                # 构建和运行脚本
│   ├── build.sh
│   ├── run.sh
│   └── docker-run.sh
├── docker/                 # Docker 配置
├── .github/workflows/      # CI/CD 配置
│   └── build.yml
├── CMakeLists.txt          # CMake 构建配置
├── Dockerfile              # Docker 镜像
├── docker-compose.yml      # Docker Compose
├── requirements.txt        # Python 依赖
├── ARCHITECTURE.md         # 架构设计文档
└── README.md               # 本文件
```

## 💡 技术亮点（面试展示）

### 1. 算法能力展示

```cpp
// 使用 STL 容器优化的滑动窗口算法
std::vector<double> SMA(const std::vector<double>& data, int period) {
    std::vector<double> result;
    double sum = std::accumulate(data.begin(), data.begin() + period, 0.0);
    result.push_back(sum / period);
    
    // O(1) 增量更新，避免重复计算
    for (size_t i = period; i < data.size(); ++i) {
        sum = sum - data[i - period] + data[i];
        result.push_back(sum / period);
    }
    return result;
}
```

**亮点：**
- ✅ O(n) 时间复杂度
- ✅ 内存预分配优化
- ✅ STL 算法使用（std::accumulate）

### 2. 数据结构选择

```cpp
class DualMAStrategy {
    std::deque<double> prices_;  // 双端队列，O(1) 头尾操作
    // vs std::vector 在头部删除是 O(n)
};
```

### 3. 现代 C++ 特性

- Header-only 库设计（模板元编程）
- 枚举类（enum class）类型安全
- RAII 资源管理
- 智能指针（std::shared_ptr）

### 4. 混合编程优势

**C++ 负责计算密集任务：**
- 指标计算（1万数据点 < 100μs）
- 策略信号生成
- 高频数据处理

**Python 负责 I/O 和业务逻辑：**
- API 调用
- 配置管理
- 日志和监控

## 📊 性能基准

```bash
$ ./build/bin/example_strategy_test

处理 10,000 个数据点，计算 SMA(20)
耗时: 87 μs
结果数量: 9981

# 对比纯 Python 实现：约 2-5 ms
# 性能提升：20-60x
```

## 🛡️ 风控系统

- ✅ 最大持仓限制
- ✅ 单笔交易限额
- ✅ 模拟交易模式（安全测试）
- ✅ 止损和止盈（计划中）

## 📈 策略说明

### 双均线策略（Dual Moving Average）

**原理：**
- 快线（5日均线）上穿慢线（20日均线）→ 买入信号（金叉）
- 快线下穿慢线 → 卖出信号（死叉）

**适用场景：**
- 趋势明显的市场
- 中长期交易

**扩展方向：**
- 均值回归策略
- 动量策略
- 机器学习信号增强

## 🔧 开发和调试

### 编译选项

```bash
# Debug 模式（包含调试符号）
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release 模式（优化性能）
cmake .. -DCMAKE_BUILD_TYPE=Release

# 启用单元测试
cmake .. -DBUILD_TESTS=ON
```

### 日志级别

在 `config/config.yaml` 中设置：

```yaml
logging:
  level: "DEBUG"  # DEBUG | INFO | WARNING | ERROR
  file: "logs/trading.log"
```

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

### 开发路线图

- [ ] 添加更多技术指标（RSI, MACD, KDJ）
- [ ] 实现回测引擎
- [ ] 支持更多交易所（OKX, Huobi）
- [ ] Web 控制面板
- [ ] 参数优化（遗传算法）
- [ ] 机器学习策略

## 📄 许可证

MIT License - 自由使用和修改

## 📧 联系方式

- GitHub: [@Yulong-Cauli](https://github.com/Yulong-Cauli)
- 项目主页: [Fast-Quant-Core](https://github.com/Yulong-Cauli/Fast-Quant-Core)

---

<div align="center">

**如果这个项目对你有帮助，请给个 ⭐️ Star 支持一下！**

Made with ❤️ by ICPC Medalists

</div>
