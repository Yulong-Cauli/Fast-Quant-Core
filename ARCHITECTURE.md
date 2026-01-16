# Fast-Quant-Core 系统架构设计

## 1. 系统架构概览

```
┌──────────────────────────────────────────────────────────────────┐
│                         用户界面层                                  │
│            (可选：简单的命令行界面或Web Dashboard)                   │
└──────────────────────────────────────────────────────────────────┘
                                ↓
┌──────────────────────────────────────────────────────────────────┐
│                      Python 应用层 (app/)                         │
│  ┌────────────────┐  ┌─────────────┐  ┌──────────────────┐     │
│  │  交易所连接器   │  │  订单管理器  │  │   盈亏统计器      │     │
│  │ (binance_api.py)│  │(order_mgr.py)│  │  (pnl_tracker.py)│     │
│  └────────────────┘  └─────────────┘  └──────────────────┘     │
└──────────────────────────────────────────────────────────────────┘
                                ↓
┌──────────────────────────────────────────────────────────────────┐
│                   Python-C++ 桥接层 (bindings/)                   │
│            使用 pybind11 实现高性能数据传递                         │
└──────────────────────────────────────────────────────────────────┘
                                ↓
┌──────────────────────────────────────────────────────────────────┐
│                   C++ 核心策略层 (core/)                          │
│  ┌──────────────┐  ┌─────────────────┐  ┌──────────────┐      │
│  │  市场数据结构 │  │   策略引擎       │  │  指标计算器   │      │
│  │ (MarketData)  │  │ (StrategyEngine) │  │ (Indicators) │      │
│  └──────────────┘  └─────────────────┘  └──────────────┘      │
│                                                                  │
│  特点：使用 STL 容器优化、模板元编程、高性能算法实现               │
└──────────────────────────────────────────────────────────────────┘
                                ↓
┌──────────────────────────────────────────────────────────────────┐
│                        数据存储层                                  │
│    SQLite (本地) / Redis (缓存) / CSV (历史数据)                  │
└──────────────────────────────────────────────────────────────────┘
```

## 2. 技术选型说明

### 2.1 混合架构优势
- **C++ 核心层**：展示你的算法能力和 STL 使用能力
  - 策略计算密集型任务
  - 高频数据处理
  - 指标计算（移动平均、标准差等）
  
- **Python 应用层**：快速开发和集成
  - API 调用和 HTTP 请求
  - 配置管理
  - 日志和监控
  - 简单的 Web 服务（如需要）

### 2.2 核心技术栈
- **C++17/20**：现代 C++ 特性
- **CMake**：构建系统
- **pybind11**：Python-C++ 绑定
- **Python 3.9+**：应用层开发
- **ccxt / python-binance**：交易所 API 封装
- **Docker**：容器化部署
- **GitHub Actions**：CI/CD 自动化

## 3. 模块详细设计

### 3.1 C++ 核心模块 (core/)

```cpp
// 市场数据结构
struct Tick {
    std::string symbol;
    double price;
    double volume;
    long long timestamp;
};

// 双均线策略引擎
class DualMAStrategy {
private:
    std::deque<double> prices;  // 使用 deque 存储价格历史
    int fast_period;
    int slow_period;
    
public:
    // 计算移动平均
    double calculateMA(int period);
    
    // 生成交易信号
    enum Signal { BUY, SELL, HOLD };
    Signal generateSignal(const Tick& tick);
};

// 指标计算器
class Indicators {
public:
    static double SMA(const std::vector<double>& data, int period);
    static double EMA(const std::vector<double>& data, int period);
    static double StdDev(const std::vector<double>& data);
};
```

### 3.2 Python 应用模块 (app/)

```python
# binance_api.py - 交易所连接器
class BinanceConnector:
    def __init__(self, api_key, api_secret):
        self.client = Client(api_key, api_secret)
    
    def subscribe_ticker(self, symbol, callback):
        """订阅实时行情"""
        pass
    
    def place_order(self, symbol, side, quantity, order_type):
        """下单"""
        pass

# strategy_runner.py - 策略运行器
class StrategyRunner:
    def __init__(self, strategy_core, connector):
        self.strategy = strategy_core  # C++ 策略对象
        self.connector = connector
    
    def on_tick(self, tick):
        """处理新的 tick 数据"""
        signal = self.strategy.generate_signal(tick)
        if signal == "BUY":
            self.connector.place_order(...)
```

## 4. 开发阶段规划

### 第一阶段（1周）：环境搭建与行情获取 ✓
- [x] 项目结构搭建
- [x] CMake 配置
- [x] pybind11 集成
- [x] Binance API 基础连接
- [x] 实时行情订阅与展示
- [x] Docker 环境配置

### 第二阶段（1周）：策略核心开发
- [ ] C++ 双均线策略实现
- [ ] 指标计算模块（SMA, EMA）
- [ ] 回测引擎基础框架
- [ ] 单元测试（Google Test）

### 第三阶段（1周）：交易执行与风控
- [ ] 自动下单模块
- [ ] 仓位管理
- [ ] 基础风控（最大持仓、止损）
- [ ] 盈亏统计模块

### 第四阶段（1周）：部署与优化
- [ ] GitHub Actions CI/CD
- [ ] Docker 生产镜像优化
- [ ] 日志和监控系统
- [ ] 性能优化和压力测试
- [ ] 文档完善

## 5. 展示亮点（面试时）

### 5.1 算法能力展示
- 使用 STL 容器的高效数据结构选择
- 滑动窗口算法优化移动平均计算
- 模板元编程实现通用指标计算器

### 5.2 工程能力展示
- 现代 C++ 项目组织（CMake, namespace, RAII）
- Python-C++ 混合编程（pybind11）
- 容器化部署（Docker multi-stage build）
- CI/CD 自动化测试和部署

### 5.3 金融领域理解
- 经典量化策略实现（双均线、均值回归）
- 风险管理意识（仓位控制、止损）
- 实时数据处理和低延迟优化

## 6. 性能优化点

1. **内存管理**：使用 `std::vector` 预分配、`std::deque` 避免频繁移动
2. **算法优化**：移动平均的增量计算，避免重复累加
3. **并发处理**：C++11 线程池处理多品种策略
4. **数据结构**：使用 `std::unordered_map` 快速查找
5. **编译优化**：-O3 优化、Link Time Optimization (LTO)

## 7. 注意事项

1. **API 限制**：Binance API 有频率限制，需要实现请求队列
2. **时区处理**：统一使用 UTC 时间戳
3. **精度问题**：金融计算使用 double 或定点数
4. **错误处理**：网络断线、API 错误的重试机制
5. **安全性**：API Key 使用环境变量，不要硬编码

## 8. 参考资源

- [Binance API 文档](https://binance-docs.github.io/apidocs/)
- [pybind11 文档](https://pybind11.readthedocs.io/)
- [CMake 教程](https://cmake.org/cmake/help/latest/guide/tutorial/)
- [量化交易策略](https://www.quantstart.com/)
