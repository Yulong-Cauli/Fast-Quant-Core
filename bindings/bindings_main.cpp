#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../core/include/market_data.h"
#include "../core/include/indicators.h"
#include "../core/include/strategy.h"

namespace py = pybind11;
using namespace fastquant;

/**
 * @brief Python 绑定模块
 * 
 * 将 C++ 核心库导出到 Python，实现高性能混合编程
 */
PYBIND11_MODULE(fastquant_cpp, m) {
    m.doc() = "FastQuant 核心策略引擎 - Python 绑定";
    
    // ==================== 数据结构 ====================
    py::class_<Tick>(m, "Tick")
        .def(py::init<>())
        .def(py::init<const std::string&, double, double, long long>(),
             py::arg("symbol"), py::arg("price"), py::arg("volume"), py::arg("timestamp"))
        .def_readwrite("symbol", &Tick::symbol)
        .def_readwrite("price", &Tick::price)
        .def_readwrite("volume", &Tick::volume)
        .def_readwrite("timestamp", &Tick::timestamp)
        .def("__repr__", [](const Tick& t) {
            return "<Tick " + t.symbol + " price=" + std::to_string(t.price) + ">";
        });
    
    py::class_<Candle>(m, "Candle")
        .def(py::init<>())
        .def_readwrite("symbol", &Candle::symbol)
        .def_readwrite("timestamp", &Candle::timestamp)
        .def_readwrite("open", &Candle::open)
        .def_readwrite("high", &Candle::high)
        .def_readwrite("low", &Candle::low)
        .def_readwrite("close", &Candle::close)
        .def_readwrite("volume", &Candle::volume);
    
    // ==================== 枚举类型 ====================
    py::enum_<Signal>(m, "Signal")
        .value("BUY", Signal::BUY)
        .value("SELL", Signal::SELL)
        .value("HOLD", Signal::HOLD)
        .export_values();
    
    py::enum_<OrderSide>(m, "OrderSide")
        .value("BUY", OrderSide::BUY)
        .value("SELL", OrderSide::SELL)
        .export_values();
    
    py::enum_<OrderType>(m, "OrderType")
        .value("MARKET", OrderType::MARKET)
        .value("LIMIT", OrderType::LIMIT)
        .export_values();
    
    // ==================== 指标计算器 ====================
    py::class_<Indicators>(m, "Indicators")
        .def_static("SMA", &Indicators::SMA,
                   "计算简单移动平均 (O(n) 滑动窗口)",
                   py::arg("data"), py::arg("period"))
        .def_static("EMA", &Indicators::EMA,
                   "计算指数移动平均",
                   py::arg("data"), py::arg("period"))
        .def_static("StdDev", &Indicators::StdDev,
                   "计算标准差（单窗口）",
                   py::arg("data"))
        .def_static("RollingStdDev", &Indicators::RollingStdDev,
                   "计算滚动标准差 (O(n) 滑动窗口优化)",
                   py::arg("data"), py::arg("period"))
        .def_static("BollingerBands", &Indicators::BollingerBands,
                   "计算布林带 (O(n) 优化版本)",
                   py::arg("data"), py::arg("period"), 
                   py::arg("std_dev_multiplier") = 2.0);
    
    // ==================== 双均线策略 ====================
    py::class_<DualMAStrategy>(m, "DualMAStrategy")
        .def(py::init<const std::string&, int, int>(),
             py::arg("symbol"), py::arg("fast_period"), py::arg("slow_period"),
             "创建双均线策略\n\n"
             "Args:\n"
             "    symbol: 交易对符号，如 'BTCUSDT'\n"
             "    fast_period: 快线周期，如 5\n"
             "    slow_period: 慢线周期，如 20")
        .def("onTick", &DualMAStrategy::onTick,
             "处理新的 Tick 数据，返回交易信号",
             py::arg("tick"))
        .def("backtestOnTicks", &DualMAStrategy::backtestOnTicks,
             "批量回测历史数据",
             py::arg("ticks"))
        .def("getFastMA", &DualMAStrategy::getFastMA,
             "获取当前快线值")
        .def("getSlowMA", &DualMAStrategy::getSlowMA,
             "获取当前慢线值")
        .def("getSymbol", &DualMAStrategy::getSymbol,
             "获取交易对")
        .def("getFastPeriod", &DualMAStrategy::getFastPeriod)
        .def("getSlowPeriod", &DualMAStrategy::getSlowPeriod);
    
    // ==================== 版本信息 ====================
    m.attr("__version__") = "0.1.0";
}
