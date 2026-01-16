#pragma once

#include <string>
#include <vector>

namespace fastquant {

/**
 * @brief 市场 Tick 数据结构
 */
struct Tick {
    std::string symbol;      // 交易对符号，如 "BTCUSDT"
    double price;            // 最新价格
    double volume;           // 成交量
    long long timestamp;     // 时间戳（毫秒）
    
    Tick() : price(0.0), volume(0.0), timestamp(0) {}
    Tick(const std::string& sym, double p, double v, long long ts)
        : symbol(sym), price(p), volume(v), timestamp(ts) {}
};

/**
 * @brief K线数据结构
 */
struct Candle {
    std::string symbol;
    long long timestamp;
    double open;
    double high;
    double low;
    double close;
    double volume;
    
    Candle() : timestamp(0), open(0), high(0), low(0), close(0), volume(0) {}
};

/**
 * @brief 订单类型
 */
enum class OrderType {
    MARKET,  // 市价单
    LIMIT    // 限价单
};

/**
 * @brief 订单方向
 */
enum class OrderSide {
    BUY,
    SELL
};

/**
 * @brief 交易信号
 */
enum class Signal {
    BUY,
    SELL,
    HOLD
};

/**
 * @brief 订单结构
 */
struct Order {
    std::string symbol;
    OrderSide side;
    OrderType type;
    double quantity;
    double price;  // 限价单使用
    long long timestamp;
    
    Order() : side(OrderSide::BUY), type(OrderType::MARKET), 
              quantity(0), price(0), timestamp(0) {}
};

} // namespace fastquant
