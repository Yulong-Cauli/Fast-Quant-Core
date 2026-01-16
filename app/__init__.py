"""
FastQuant - 轻量级量化交易系统

混合 C++/Python 架构，展示高性能量化交易开发能力
"""

__version__ = "0.1.0"
__author__ = "FastQuant Team"

# 导出主要模块
from .binance_connector import BinanceConnector
from .strategy_runner import StrategyRunner
from .pnl_tracker import PnLTracker

__all__ = [
    'BinanceConnector',
    'StrategyRunner', 
    'PnLTracker',
]
