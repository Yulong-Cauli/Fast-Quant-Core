#!/usr/bin/env python3
"""
快速演示脚本 - 展示 C++ 核心库功能

不需要 Binance API，纯本地计算演示
"""

import sys
from pathlib import Path

# 添加项目路径
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from app import fastquant_cpp
    print("="*60)
    print("FastQuant 核心功能演示")
    print("="*60)
    print()
    
    # 1. 技术指标计算
    print("1️⃣  技术指标计算")
    print("-" * 60)
    
    prices = [100.0, 102.0, 101.0, 103.0, 105.0, 104.0, 106.0, 108.0, 107.0, 110.0]
    print(f"价格序列: {prices}")
    
    sma = fastquant_cpp.Indicators.SMA(prices, 5)
    print(f"5周期 SMA: {[f'{x:.2f}' for x in sma]}")
    
    stddev = fastquant_cpp.Indicators.StdDev(prices)
    print(f"标准差: {stddev:.2f}")
    print()
    
    # 2. 双均线策略
    print("2️⃣  双均线策略测试")
    print("-" * 60)
    
    strategy = fastquant_cpp.DualMAStrategy("BTCUSDT", 5, 20)
    print(f"策略: {strategy.getSymbol()} - 快线{strategy.getFastPeriod()}/慢线{strategy.getSlowPeriod()}")
    print()
    
    # 模拟价格序列
    test_prices = [
        100, 101, 102, 103, 104,  # 前5个
        105, 106, 107, 108, 109,  # 10个
        110, 111, 112, 113, 114,  # 15个
        115, 116, 117, 118, 119,  # 20个
        120, 121, 122, 123, 124,  # 25个 - 应该触发买入
        123, 122, 121, 120, 119,  # 下跌
        118, 117, 116, 115, 114   # 继续下跌 - 应该触发卖出
    ]
    
    print("处理 Tick 数据...")
    buy_count = 0
    sell_count = 0
    
    for i, price in enumerate(test_prices):
        tick = fastquant_cpp.Tick("BTCUSDT", float(price), 1.0, 1640000000000 + i * 60000)
        signal = strategy.onTick(tick)
        
        if signal == fastquant_cpp.Signal.BUY:
            print(f"  ✅ 买入信号 @ {price:.2f} (快线={strategy.getFastMA():.2f}, 慢线={strategy.getSlowMA():.2f})")
            buy_count += 1
        elif signal == fastquant_cpp.Signal.SELL:
            print(f"  ❌ 卖出信号 @ {price:.2f} (快线={strategy.getFastMA():.2f}, 慢线={strategy.getSlowMA():.2f})")
            sell_count += 1
    
    print()
    print(f"统计: 买入信号 {buy_count} 次, 卖出信号 {sell_count} 次")
    print()
    
    # 3. 批量回测
    print("3️⃣  批量回测模式")
    print("-" * 60)
    
    strategy2 = fastquant_cpp.DualMAStrategy("ETHUSDT", 3, 10)
    ticks = [fastquant_cpp.Tick("ETHUSDT", float(p), 1.0, 1640000000000 + i * 60000) 
             for i, p in enumerate(test_prices)]
    
    signals = strategy2.backtestOnTicks(ticks)
    buy_signals = sum(1 for s in signals if s == fastquant_cpp.Signal.BUY)
    sell_signals = sum(1 for s in signals if s == fastquant_cpp.Signal.SELL)
    
    print(f"回测 {len(ticks)} 个 Tick")
    print(f"买入信号: {buy_signals} 次")
    print(f"卖出信号: {sell_signals} 次")
    print()
    
    print("="*60)
    print("✅ 演示完成！")
    print("="*60)
    print()
    print("💡 面试展示重点:")
    print("  - C++ 核心计算，Python 调用")
    print("  - STL 容器优化（vector, deque）")
    print("  - O(n) 滑动窗口算法")
    print("  - pybind11 无缝集成")
    print("  - 完整的策略框架")
    
except ImportError as e:
    print(f"❌ 错误: 无法加载 C++ 模块")
    print(f"   {e}")
    print()
    print("请先编译 C++ 核心库:")
    print("  ./scripts/build.sh")
    sys.exit(1)
