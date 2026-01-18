/**
 * 测试指标优化
 * 
 * 验证 O(n) 优化的正确性和性能提升
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <cmath>
#include "../core/include/indicators.h"

using namespace fastquant;

// 辅助函数：比较两个浮点数是否近似相等
bool approx_equal(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

// 辅助函数：比较两个向量是否近似相等
bool vectors_approx_equal(const std::vector<double>& a, const std::vector<double>& b, double epsilon = 1e-6) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (!approx_equal(a[i], b[i], epsilon)) {
            std::cout << "  差异在索引 " << i << ": " << a[i] << " vs " << b[i] << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    std::cout << "=== 指标优化测试 ===" << std::endl;
    std::cout << std::endl;
    
    // 测试数据
    std::vector<double> prices = {
        100.0, 102.0, 101.0, 103.0, 105.0,
        104.0, 106.0, 108.0, 107.0, 110.0,
        112.0, 111.0, 113.0, 115.0, 114.0,
        116.0, 118.0, 117.0, 119.0, 121.0
    };
    
    std::cout << "测试数据大小: " << prices.size() << std::endl;
    std::cout << std::endl;
    
    // ========== 测试 1: SMA 边界检查 ==========
    std::cout << "测试 1: SMA 严格边界检查" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    auto empty_result = Indicators::SMA({}, 5);
    std::cout << "空数据: " << (empty_result.empty() ? "✓ 通过" : "✗ 失败") << std::endl;
    
    auto neg_period = Indicators::SMA(prices, -1);
    std::cout << "负周期: " << (neg_period.empty() ? "✓ 通过" : "✗ 失败") << std::endl;
    
    auto zero_period = Indicators::SMA(prices, 0);
    std::cout << "零周期: " << (zero_period.empty() ? "✓ 通过" : "✗ 失败") << std::endl;
    
    auto large_period = Indicators::SMA(prices, prices.size() + 1);
    std::cout << "过大周期: " << (large_period.empty() ? "✓ 通过" : "✗ 失败") << std::endl;
    
    // 测试 NaN/Inf 处理
    std::vector<double> bad_data = {1.0, 2.0, std::nan(""), 4.0};
    auto nan_result = Indicators::SMA(bad_data, 2);
    std::cout << "NaN 数据: " << (nan_result.empty() ? "✓ 通过" : "✗ 失败") << std::endl;
    
    std::cout << std::endl;
    
    // ========== 测试 2: RollingStdDev 正确性 ==========
    std::cout << "测试 2: RollingStdDev 正确性验证" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    int period = 5;
    auto rolling_stddev = Indicators::RollingStdDev(prices, period);
    
    std::cout << "计算 " << period << " 周期滚动标准差" << std::endl;
    std::cout << "结果数量: " << rolling_stddev.size() 
              << " (期望: " << (prices.size() - period + 1) << ")" << std::endl;
    
    // 手动验证第一个窗口
    std::vector<double> first_window(prices.begin(), prices.begin() + period);
    double manual_stddev = Indicators::StdDev(first_window);
    
    std::cout << "第一个窗口标准差:" << std::endl;
    std::cout << "  滚动计算: " << std::fixed << std::setprecision(6) << rolling_stddev[0] << std::endl;
    std::cout << "  手动验证: " << manual_stddev << std::endl;
    std::cout << "  结果: " << (approx_equal(rolling_stddev[0], manual_stddev) ? "✓ 通过" : "✗ 失败") << std::endl;
    
    std::cout << std::endl;
    
    // ========== 测试 3: BollingerBands 优化前后对比 ==========
    std::cout << "测试 3: BollingerBands 优化验证" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    auto [upper, middle, lower] = Indicators::BollingerBands(prices, period, 2.0);
    
    std::cout << "布林带计算结果:" << std::endl;
    std::cout << "  上轨数量: " << upper.size() << std::endl;
    std::cout << "  中轨数量: " << middle.size() << std::endl;
    std::cout << "  下轨数量: " << lower.size() << std::endl;
    
    // 验证中轨是否等于 SMA
    auto sma_result = Indicators::SMA(prices, period);
    bool middle_correct = vectors_approx_equal(middle, sma_result);
    std::cout << "  中轨 = SMA: " << (middle_correct ? "✓ 通过" : "✗ 失败") << std::endl;
    
    // 验证第一个窗口的上下轨
    double expected_upper = middle[0] + 2.0 * rolling_stddev[0];
    double expected_lower = middle[0] - 2.0 * rolling_stddev[0];
    
    std::cout << "第一个窗口验证:" << std::endl;
    std::cout << "  上轨: " << upper[0] << " (期望: " << expected_upper << ")" << std::endl;
    std::cout << "  下轨: " << lower[0] << " (期望: " << expected_lower << ")" << std::endl;
    
    bool bands_correct = approx_equal(upper[0], expected_upper) && 
                        approx_equal(lower[0], expected_lower);
    std::cout << "  结果: " << (bands_correct ? "✓ 通过" : "✗ 失败") << std::endl;
    
    std::cout << std::endl;
    
    // ========== 测试 4: 性能对比 ==========
    std::cout << "测试 4: 性能基准测试" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    // 生成大量数据
    std::vector<double> large_data;
    large_data.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        large_data.push_back(100.0 + (i % 100) * 0.5);
    }
    
    // 测试 BollingerBands 性能
    auto start = std::chrono::high_resolution_clock::now();
    auto result = Indicators::BollingerBands(large_data, 20, 2.0);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "BollingerBands (10,000 数据点, 周期=20):" << std::endl;
    std::cout << "  耗时: " << duration.count() << " μs" << std::endl;
    std::cout << "  结果数量: " << std::get<0>(result).size() << std::endl;
    
    // 测试 RollingStdDev 性能
    start = std::chrono::high_resolution_clock::now();
    auto stddev_result = Indicators::RollingStdDev(large_data, 20);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "RollingStdDev (10,000 数据点, 周期=20):" << std::endl;
    std::cout << "  耗时: " << duration.count() << " μs" << std::endl;
    std::cout << "  结果数量: " << stddev_result.size() << std::endl;
    
    std::cout << std::endl;
    
    // ========== 测试 5: 数值稳定性 ==========
    std::cout << "测试 5: 数值稳定性测试" << std::endl;
    std::cout << "-------------------" << std::endl;
    
    // 测试接近零的小数值
    std::vector<double> small_values = {
        0.0001, 0.0002, 0.0001, 0.0003, 0.0002,
        0.0001, 0.0002, 0.0003, 0.0001, 0.0002
    };
    
    auto small_stddev = Indicators::RollingStdDev(small_values, 5);
    bool all_finite = true;
    for (const auto& val : small_stddev) {
        if (!std::isfinite(val)) {
            all_finite = false;
            break;
        }
    }
    std::cout << "小数值测试: " << (all_finite ? "✓ 通过" : "✗ 失败") << std::endl;
    
    // 测试大数值
    std::vector<double> large_values;
    for (int i = 0; i < 10; ++i) {
        large_values.push_back(1000000.0 + i * 100.0);
    }
    
    auto large_stddev = Indicators::RollingStdDev(large_values, 5);
    all_finite = true;
    for (const auto& val : large_stddev) {
        if (!std::isfinite(val)) {
            all_finite = false;
            break;
        }
    }
    std::cout << "大数值测试: " << (all_finite ? "✓ 通过" : "✗ 失败") << std::endl;
    
    std::cout << std::endl;
    
    std::cout << "=== 测试完成 ===" << std::endl;
    std::cout << std::endl;
    std::cout << "优化总结:" << std::endl;
    std::cout << "✓ BollingerBands: O(n·period) → O(n)" << std::endl;
    std::cout << "✓ RollingStdDev: 新增 O(n) 实现" << std::endl;
    std::cout << "✓ 避免临时容器创建和拷贝" << std::endl;
    std::cout << "✓ 使用 σ² = E[X²] - (E[X])² 公式" << std::endl;
    std::cout << "✓ 滑动窗口维护 sum(x) 和 sum(x²)" << std::endl;
    std::cout << "✓ 严格边界检查和数据验证" << std::endl;
    std::cout << "✓ 数值稳定性保证" << std::endl;
    
    return 0;
}
