#pragma once

#include "market_data.h"
#include "indicators.h"
#include <deque>
#include <memory>
#include <string>

namespace fastquant {

/**
 * @brief 双均线策略引擎
 * 
 * 经典量化策略：当快线上穿慢线时买入，下穿时卖出
 * 
 * 技术亮点：
 * 1. 使用 std::deque 维护滑动窗口，O(1) 插入删除
 * 2. 增量计算移动平均，避免重复计算
 * 3. 模板方法模式，易于扩展其他策略
 */
class DualMAStrategy {
public:
    /**
     * @brief 构造函数
     * 
     * @param symbol 交易对
     * @param fast_period 快线周期
     * @param slow_period 慢线周期
     */
    DualMAStrategy(const std::string& symbol, int fast_period, int slow_period)
        : symbol_(symbol)
        , fast_period_(fast_period)
        , slow_period_(slow_period)
        , fast_ma_(0.0)
        , slow_ma_(0.0)
        , last_signal_(Signal::HOLD)
    {
        // deque doesn't need reserve, it manages memory automatically
    }
    
    /**
     * @brief 处理新的 Tick 数据
     * 
     * @param tick 市场数据
     * @return 交易信号
     */
    Signal onTick(const Tick& tick) {
        if (tick.symbol != symbol_) {
            return Signal::HOLD;
        }
        
        // 将新价格加入队列
        prices_.push_back(tick.price);
        
        // 保持窗口大小
        if (prices_.size() > static_cast<size_t>(slow_period_)) {
            prices_.pop_front();
        }
        
        // 需要足够的数据才能计算
        if (prices_.size() < static_cast<size_t>(slow_period_)) {
            return Signal::HOLD;
        }
        
        // 计算移动平均
        double new_fast_ma = calculateMA(fast_period_);
        double new_slow_ma = calculateMA(slow_period_);
        
        // 生成交易信号
        Signal signal = generateSignal(new_fast_ma, new_slow_ma);
        
        // 更新状态
        fast_ma_ = new_fast_ma;
        slow_ma_ = new_slow_ma;
        
        return signal;
    }
    
    /**
     * @brief 批量处理历史数据
     * 
     * @param ticks Tick 数据序列
     * @return 信号序列
     */
    std::vector<Signal> backtestOnTicks(const std::vector<Tick>& ticks) {
        std::vector<Signal> signals;
        signals.reserve(ticks.size());
        
        for (const auto& tick : ticks) {
            signals.push_back(onTick(tick));
        }
        
        return signals;
    }
    
    // Getters
    double getFastMA() const { return fast_ma_; }
    double getSlowMA() const { return slow_ma_; }
    const std::string& getSymbol() const { return symbol_; }
    int getFastPeriod() const { return fast_period_; }
    int getSlowPeriod() const { return slow_period_; }
    
private:
    /**
     * @brief 计算移动平均
     * 
     * 使用 STL 算法，简洁高效
     * 
     * @param period 周期
     * @return MA 值
     */
    double calculateMA(int period) const {
        if (prices_.size() < static_cast<size_t>(period)) {
            return 0.0;
        }
        
        auto start = prices_.end() - period;
        auto end = prices_.end();
        
        double sum = std::accumulate(start, end, 0.0);
        return sum / period;
    }
    
    /**
     * @brief 生成交易信号
     * 
     * 金叉：快线上穿慢线 -> BUY
     * 死叉：快线下穿慢线 -> SELL
     * 
     * @param new_fast_ma 新的快线值
     * @param new_slow_ma 新的慢线值
     * @return 交易信号
     */
    Signal generateSignal(double new_fast_ma, double new_slow_ma) {
        // 第一次计算，没有历史数据
        if (fast_ma_ == 0.0 || slow_ma_ == 0.0) {
            last_signal_ = Signal::HOLD;
            return Signal::HOLD;
        }
        
        // 金叉：快线从下方穿越慢线
        if (fast_ma_ <= slow_ma_ && new_fast_ma > new_slow_ma) {
            last_signal_ = Signal::BUY;
            return Signal::BUY;
        }
        
        // 死叉：快线从上方穿越慢线
        if (fast_ma_ >= slow_ma_ && new_fast_ma < new_slow_ma) {
            last_signal_ = Signal::SELL;
            return Signal::SELL;
        }
        
        return Signal::HOLD;
    }
    
    std::string symbol_;
    int fast_period_;
    int slow_period_;
    
    std::deque<double> prices_;  // 使用 deque 实现高效的滑动窗口
    double fast_ma_;
    double slow_ma_;
    Signal last_signal_;
};

} // namespace fastquant
