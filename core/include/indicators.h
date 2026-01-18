#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <optional>
#include <limits>

namespace fastquant {

/**
 * @brief 技术指标计算器
 * 
 * 展示 C++ STL 和算法能力的核心模块
 * 优化重点：O(n) 滑动窗口算法，避免容器拷贝
 */
class Indicators {
public:
    /**
     * @brief 简单移动平均 (Simple Moving Average)
     * 
     * 使用滑动窗口算法，时间复杂度 O(n)
     * 
     * @param data 价格序列
     * @param period 周期，必须 > 0 且 <= data.size()
     * @return 移动平均值序列，如果参数非法返回空
     */
    static std::vector<double> SMA(const std::vector<double>& data, int period) {
        std::vector<double> result;
        
        // 严格边界检查
        if (period <= 0 || data.empty() || 
            static_cast<size_t>(period) > data.size()) {
            return result;
        }
        
        // 检查数据有效性（避免 NaN/Inf）
        for (const auto& value : data) {
            if (!std::isfinite(value)) {
                return result;
            }
        }
        
        result.reserve(data.size() - period + 1);
        
        // 第一个窗口的和
        double sum = std::accumulate(data.begin(), data.begin() + period, 0.0);
        result.push_back(sum / period);
        
        // 滑动窗口：O(1) 增量更新
        for (size_t i = period; i < data.size(); ++i) {
            sum = sum - data[i - period] + data[i];
            result.push_back(sum / period);
        }
        
        return result;
    }
    
    /**
     * @brief 指数移动平均 (Exponential Moving Average)
     * 
     * @param data 价格序列
     * @param period 周期，必须 > 0 且 <= data.size()
     * @return EMA 值序列，如果参数非法返回空
     */
    static std::vector<double> EMA(const std::vector<double>& data, int period) {
        std::vector<double> result;
        
        // 严格边界检查
        if (period <= 0 || data.empty() || 
            static_cast<size_t>(period) > data.size()) {
            return result;
        }
        
        // 检查数据有效性
        for (const auto& value : data) {
            if (!std::isfinite(value)) {
                return result;
            }
        }
        
        result.reserve(data.size());
        double multiplier = 2.0 / (period + 1);
        
        // 第一个 EMA 值使用 SMA
        double ema = std::accumulate(data.begin(), 
                                     data.begin() + period,
                                     0.0) / period;
        result.push_back(ema);
        
        // 后续 EMA 值：递推计算
        for (size_t i = 1; i < data.size(); ++i) {
            ema = (data[i] - ema) * multiplier + ema;
            result.push_back(ema);
        }
        
        return result;
    }
    
    /**
     * @brief 标准差计算（单窗口版本，用于向后兼容）
     * 
     * @param data 数据序列
     * @return 标准差
     */
    static double StdDev(const std::vector<double>& data) {
        if (data.empty()) {
            return 0.0;
        }
        
        // 使用 Welford's online algorithm 提高数值稳定性
        double mean = 0.0;
        double m2 = 0.0;
        size_t count = 0;
        
        for (const auto& value : data) {
            if (!std::isfinite(value)) {
                return 0.0;
            }
            count++;
            double delta = value - mean;
            mean += delta / count;
            double delta2 = value - mean;
            m2 += delta * delta2;
        }
        
        if (count < 2) {
            return 0.0;
        }
        
        return std::sqrt(m2 / count);
    }
    
    /**
     * @brief 滚动标准差计算（O(n) 复杂度）
     * 
     * 使用滑动窗口维护 sum(x) 和 sum(x²)，利用公式：
     * σ² = E[X²] - (E[X])² = (sum(x²)/n) - (sum(x)/n)²
     * 
     * @param data 价格序列
     * @param period 周期
     * @return 每个窗口的标准差序列
     */
    static std::vector<double> RollingStdDev(const std::vector<double>& data, int period) {
        std::vector<double> result;
        
        if (period <= 0 || data.empty() || 
            static_cast<size_t>(period) > data.size()) {
            return result;
        }
        
        // 检查数据有效性
        for (const auto& value : data) {
            if (!std::isfinite(value)) {
                return result;
            }
        }
        
        result.reserve(data.size() - period + 1);
        
        // 初始化第一个窗口的 sum(x) 和 sum(x²)
        double sum_x = 0.0;
        double sum_x2 = 0.0;
        
        for (int i = 0; i < period; ++i) {
            sum_x += data[i];
            sum_x2 += data[i] * data[i];
        }
        
        // 计算第一个窗口的标准差
        double mean = sum_x / period;
        double variance = (sum_x2 / period) - (mean * mean);
        result.push_back(std::sqrt(std::max(0.0, variance)));  // 避免负数（浮点误差）
        
        // 滑动窗口：O(1) 更新 sum(x) 和 sum(x²)
        for (size_t i = period; i < data.size(); ++i) {
            double old_val = data[i - period];
            double new_val = data[i];
            
            sum_x = sum_x - old_val + new_val;
            sum_x2 = sum_x2 - (old_val * old_val) + (new_val * new_val);
            
            mean = sum_x / period;
            variance = (sum_x2 / period) - (mean * mean);
            result.push_back(std::sqrt(std::max(0.0, variance)));
        }
        
        return result;
    }
    
    /**
     * @brief 布林带计算（优化到 O(n) 复杂度）
     * 
     * 使用滑动窗口同时计算 SMA 和滚动标准差，避免重复计算和临时容器创建
     * 
     * @param data 价格序列
     * @param period 周期
     * @param std_dev_multiplier 标准差倍数（默认2）
     * @return {上轨, 中轨, 下轨}
     */
    static std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> 
    BollingerBands(const std::vector<double>& data, int period, double std_dev_multiplier = 2.0) {
        std::vector<double> upper, middle, lower;
        
        // 边界检查
        if (period <= 0 || data.empty() || 
            static_cast<size_t>(period) > data.size() ||
            std_dev_multiplier < 0) {
            return {upper, middle, lower};
        }
        
        // 检查数据有效性
        for (const auto& value : data) {
            if (!std::isfinite(value)) {
                return {upper, middle, lower};
            }
        }
        
        size_t result_size = data.size() - period + 1;
        upper.reserve(result_size);
        middle.reserve(result_size);
        lower.reserve(result_size);
        
        // 初始化第一个窗口：同时维护 sum(x) 和 sum(x²)
        double sum_x = 0.0;
        double sum_x2 = 0.0;
        
        for (int i = 0; i < period; ++i) {
            sum_x += data[i];
            sum_x2 += data[i] * data[i];
        }
        
        // 计算第一个窗口的中轨和标准差
        double mean = sum_x / period;
        double variance = (sum_x2 / period) - (mean * mean);
        double std_dev = std::sqrt(std::max(0.0, variance));
        
        middle.push_back(mean);
        upper.push_back(mean + std_dev_multiplier * std_dev);
        lower.push_back(mean - std_dev_multiplier * std_dev);
        
        // 滑动窗口：O(1) 增量更新 sum(x) 和 sum(x²)
        for (size_t i = period; i < data.size(); ++i) {
            double old_val = data[i - period];
            double new_val = data[i];
            
            // 更新累加器
            sum_x = sum_x - old_val + new_val;
            sum_x2 = sum_x2 - (old_val * old_val) + (new_val * new_val);
            
            // 计算当前窗口的统计量
            mean = sum_x / period;
            variance = (sum_x2 / period) - (mean * mean);
            std_dev = std::sqrt(std::max(0.0, variance));
            
            // 存储结果
            middle.push_back(mean);
            upper.push_back(mean + std_dev_multiplier * std_dev);
            lower.push_back(mean - std_dev_multiplier * std_dev);
        }
        
        return {upper, middle, lower};
    }
};

} // namespace fastquant
