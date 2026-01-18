"""
FastQuant - 轻量级量化交易系统

混合 C++/Python 架构，展示高性能量化交易开发能力
"""

__version__ = "0.1.0"
__author__ = "FastQuant Team"

# 导出主要模块（延迟导入，避免依赖问题）
__all__ = [
    'BinanceConnector',
    'StrategyRunner', 
    'PnLTracker',
]

def __getattr__(name):
    """延迟导入，避免在没有安装依赖时导入失败"""
    if name == 'BinanceConnector':
        from .binance_connector import BinanceConnector
        return BinanceConnector
    elif name == 'StrategyRunner':
        from .strategy_runner import StrategyRunner
        return StrategyRunner
    elif name == 'PnLTracker':
        from .pnl_tracker import PnLTracker
        return PnLTracker
    raise AttributeError(f"module '{__name__}' has no attribute '{name}'")
