/**
 * 策略测试示例
 * 
 * 展示如何使用 C++ 核心库进行策略开发和测试
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include "../core/include/market_data.h"
#include "../core/include/indicators.h"
#include "../core/include/strategy.h"

using namespace fastquant;

int main() {
    std::cout << "=== FastQuant 策略测试 ===" << std::endl;
    std::cout << std::endl;
    
    // ========== 测试 1: 指标计算 ==========
    std::cout << "测试 1: 技术指标计算" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    std::vector<double> prices = {
        100.0, 102.0, 101.0, 103.0, 105.0,
        104.0, 106.0, 108.0, 107.0, 110.0,
        112.0, 111.0, 113.0, 115.0, 114.0
    };
    
    // 计算 5 周期 SMA
    auto sma5 = Indicators::SMA(prices, 5);
    std::cout << "5 周期 SMA: ";
    for (const auto& value : sma5) {
        std::cout << std::fixed << std::setprecision(2) << value << " ";
    }
    std::cout << std::endl;
    
    // 计算标准差
    double stddev = Indicators::StdDev(prices);
    std::cout << "标准差: " << std::fixed << std::setprecision(2) 
              << stddev << std::endl;
    std::cout << std::endl;
    
    // ========== 测试 2: 双均线策略 ==========
    std::cout << "测试 2: 双均线策略回测" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    // 创建策略：5 日线 vs 20 日线
    DualMAStrategy strategy("BTCUSDT", 5, 20);
    
    // 构造模拟 Tick 数据
    std::vector<Tick> ticks;
    long long base_time = 1640000000000LL;  // 2021-12-20
    
    for (size_t i = 0; i < prices.size(); ++i) {
        Tick tick("BTCUSDT", prices[i], 1.0, base_time + i * 60000);
        ticks.push_back(tick);
    }
    
    // 添加更多数据以触发信号
    std::vector<double> more_prices = {
        116.0, 118.0, 117.0, 119.0, 121.0,  // 上涨趋势
        120.0, 119.0, 118.0, 116.0, 115.0   // 下跌趋势
    };
    
    for (size_t i = 0; i < more_prices.size(); ++i) {
        Tick tick("BTCUSDT", more_prices[i], 1.0, 
                 base_time + (prices.size() + i) * 60000);
        ticks.push_back(tick);
    }
    
    // 运行回测
    std::cout << "处理 " << ticks.size() << " 个 Tick..." << std::endl;
    
    int buy_count = 0, sell_count = 0;
    for (const auto& tick : ticks) {
        Signal signal = strategy.onTick(tick);
        
        if (signal == Signal::BUY) {
            std::cout << "✅ BUY 信号 @ " << tick.price 
                     << " (快线=" << strategy.getFastMA() 
                     << ", 慢线=" << strategy.getSlowMA() << ")" << std::endl;
            buy_count++;
        }
        else if (signal == Signal::SELL) {
            std::cout << "❌ SELL 信号 @ " << tick.price 
                     << " (快线=" << strategy.getFastMA() 
                     << ", 慢线=" << strategy.getSlowMA() << ")" << std::endl;
            sell_count++;
        }
    }
    
    std::cout << std::endl;
    std::cout << "回测结果: " << std::endl;
    std::cout << "  买入信号: " << buy_count << std::endl;
    std::cout << "  卖出信号: " << sell_count << std::endl;
    std::cout << "  最终快线: " << strategy.getFastMA() << std::endl;
    std::cout << "  最终慢线: " << strategy.getSlowMA() << std::endl;
    std::cout << std::endl;
    
    // ========== 性能展示 ==========
    std::cout << "测试 3: 性能测试" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    // 生成大量数据
    std::vector<double> large_data;
    large_data.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        large_data.push_back(100.0 + (i % 100) * 0.5);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    auto sma_result = Indicators::SMA(large_data, 20);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "处理 10,000 个数据点，计算 SMA(20)" << std::endl;
    std::cout << "耗时: " << duration.count() << " μs" << std::endl;
    std::cout << "结果数量: " << sma_result.size() << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== 测试完成 ===" << std::endl;
    std::cout << "\n面试亮点:" << std::endl;
    std::cout << "✓ 使用 STL 容器 (vector, deque) 优化内存管理" << std::endl;
    std::cout << "✓ 滑动窗口算法实现 O(n) 移动平均计算" << std::endl;
    std::cout << "✓ Header-only 库设计，易于集成" << std::endl;
    std::cout << "✓ 现代 C++ 特性：智能指针、枚举类、右值引用" << std::endl;
    std::cout << "✓ 完整的策略回测框架" << std::endl;
    
    return 0;
}
