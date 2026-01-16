#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>

namespace fastquant {

/**
 * @brief 技术指标计算器
 * 
 * 展示 C++ STL 和算法能力的核心模块
 */
class Indicators {
public:
    /**
     * @brief 简单移动平均 (Simple Moving Average)
     * 
     * 使用滑动窗口算法，时间复杂度 O(n)
     * 
     * @param data 价格序列
     * @param period 周期
     * @return 移动平均值序列
     */
    static std::vector<double> SMA(const std::vector<double>& data, int period) {
        std::vector<double> result;
        if (data.size() < static_cast<size_t>(period) || period <= 0) {
            return result;
        }
        
        result.reserve(data.size() - period + 1);
        
        // 第一个窗口的和
        double sum = std::accumulate(data.begin(), data.begin() + period, 0.0);
        result.push_back(sum / period);
        
        // 滑动窗口：移除最老的，加入最新的
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
     * @param period 周期
     * @return EMA 值序列
     */
    static std::vector<double> EMA(const std::vector<double>& data, int period) {
        std::vector<double> result;
        if (data.empty() || period <= 0) {
            return result;
        }
        
        result.reserve(data.size());
        double multiplier = 2.0 / (period + 1);
        
        // 第一个 EMA 值使用 SMA
        double ema = std::accumulate(data.begin(), 
                                     data.begin() + std::min(period, static_cast<int>(data.size())),
                                     0.0) / std::min(period, static_cast<int>(data.size()));
        result.push_back(ema);
        
        // 后续 EMA 值
        for (size_t i = 1; i < data.size(); ++i) {
            ema = (data[i] - ema) * multiplier + ema;
            result.push_back(ema);
        }
        
        return result;
    }
    
    /**
     * @brief 标准差计算
     * 
     * @param data 数据序列
     * @return 标准差
     */
    static double StdDev(const std::vector<double>& data) {
        if (data.empty()) {
            return 0.0;
        }
        
        double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        double variance = 0.0;
        
        for (const auto& value : data) {
            double diff = value - mean;
            variance += diff * diff;
        }
        
        return std::sqrt(variance / data.size());
    }
    
    /**
     * @brief 布林带计算
     * 
     * @param data 价格序列
     * @param period 周期
     * @param std_dev_multiplier 标准差倍数（默认2）
     * @return {上轨, 中轨, 下轨}
     */
    static std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> 
    BollingerBands(const std::vector<double>& data, int period, double std_dev_multiplier = 2.0) {
        auto middle = SMA(data, period);
        std::vector<double> upper, lower;
        
        if (middle.empty()) {
            return {upper, middle, lower};
        }
        
        upper.reserve(middle.size());
        lower.reserve(middle.size());
        
        for (size_t i = 0; i < middle.size(); ++i) {
            size_t start = i;
            size_t end = i + period;
            std::vector<double> window(data.begin() + start, data.begin() + end);
            double std_dev = StdDev(window);
            
            upper.push_back(middle[i] + std_dev_multiplier * std_dev);
            lower.push_back(middle[i] - std_dev_multiplier * std_dev);
        }
        
        return {upper, middle, lower};
    }
};

} // namespace fastquant
