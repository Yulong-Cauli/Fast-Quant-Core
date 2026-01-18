"""
策略运行器

连接交易所和策略引擎，实现自动化交易
"""

import logging
import time
from typing import Optional, Dict, Any
from datetime import datetime

# 尝试导入 C++ 模块
try:
    from . import fastquant_cpp
    Signal = fastquant_cpp.Signal
    CPP_MODULE_AVAILABLE = True
except ImportError:
    CPP_MODULE_AVAILABLE = False
    logging.warning("C++ 模块未编译，策略运行器功能受限")
    
    # 定义纯 Python 的 Signal
    class Signal:
        BUY = "BUY"
        SELL = "SELL"
        HOLD = "HOLD"


class StrategyRunner:
    """
    策略运行器
    
    职责：
    1. 接收实时行情
    2. 调用 C++ 策略引擎
    3. 根据信号执行交易
    4. 记录交易日志
    """
    
    def __init__(self, strategy, connector, config: Dict[str, Any]):
        """
        初始化策略运行器
        
        Args:
            strategy: C++ 策略对象（DualMAStrategy）
            connector: 交易所连接器（BinanceConnector）
            config: 配置字典
        """
        self.logger = logging.getLogger(__name__)
        self.strategy = strategy
        self.connector = connector
        self.config = config
        
        # 交易状态
        self.position: float = 0.0  # 当前持仓（正为多仓，负为空仓）
        self.is_running: bool = False
        self.last_signal: str = "HOLD"
        
        # 风控参数
        self.max_position: float = config.get('max_position', 1.0)
        self.trade_quantity: float = config.get('trade_quantity', 0.001)
        self.enable_trading: bool = config.get('enable_trading', False)
        
        self.logger.info(f"策略运行器初始化完成，交易模式: {'真实' if self.enable_trading else '模拟'}")
    
    def start(self, symbol: str):
        """
        启动策略
        
        Args:
            symbol: 交易对
        """
        if not CPP_MODULE_AVAILABLE:
            self.logger.error("C++ 模块未加载，无法启动策略")
            return
        
        self.is_running = True
        self.logger.info(f"开始运行策略: {symbol}")
        
        # 订阅行情
        self.connector.subscribe_ticker(symbol, self.on_tick)
    
    def stop(self):
        """停止策略"""
        self.is_running = False
        self.logger.info("策略已停止")
    
    def on_tick(self, tick):
        """
        处理新的 Tick 数据
        
        Args:
            tick: Tick 对象（C++）或字典（Python）
        """
        if not self.is_running:
            return
        
        try:
            # 调用 C++ 策略引擎生成信号
            signal = self.strategy.onTick(tick)
            
            # 获取 MA 值用于日志
            fast_ma = self.strategy.getFastMA()
            slow_ma = self.strategy.getSlowMA()
            
            # 获取价格
            if CPP_MODULE_AVAILABLE:
                price = tick.price
                symbol = tick.symbol
            else:
                price = tick['price']
                symbol = tick['symbol']
            
            # 信号变化时输出日志
            signal_name = self._signal_to_string(signal)
            if signal_name != "HOLD":
                self.logger.info(
                    f"[{datetime.now().strftime('%H:%M:%S')}] "
                    f"{symbol} @ {price:.2f} | "
                    f"快线={fast_ma:.2f} 慢线={slow_ma:.2f} | "
                    f"信号={signal_name}"
                )
                
                # 执行交易
                if signal_name != self.last_signal:
                    self.execute_trade(symbol, signal_name, price)
                    self.last_signal = signal_name
        
        except Exception as e:
            self.logger.error(f"处理 Tick 数据出错: {e}", exc_info=True)
    
    def execute_trade(self, symbol: str, signal: str, price: float):
        """
        执行交易
        
        Args:
            symbol: 交易对
            signal: 信号（BUY/SELL/HOLD）
            price: 当前价格
        """
        if signal == "HOLD":
            return
        
        # 风控检查
        if signal == "BUY" and self.position >= self.max_position:
            self.logger.warning(f"持仓已达上限 {self.max_position}，忽略买入信号")
            return
        
        if signal == "SELL" and self.position <= -self.max_position:
            self.logger.warning(f"持仓已达下限 {-self.max_position}，忽略卖出信号")
            return
        
        # 计算交易方向和数量
        side = "BUY" if signal == "BUY" else "SELL"
        quantity = self.trade_quantity
        
        self.logger.info(
            f"{'[真实交易]' if self.enable_trading else '[模拟交易]'} "
            f"{side} {quantity} {symbol} @ {price:.2f}"
        )
        
        # 真实交易模式
        if self.enable_trading:
            try:
                order = self.connector.place_order(
                    symbol=symbol,
                    side=side,
                    quantity=quantity,
                    order_type='MARKET'
                )
                
                if order:
                    self.logger.info(f"订单执行成功: {order.get('orderId', 'N/A')}")
                    # 更新持仓
                    if side == "BUY":
                        self.position += quantity
                    else:
                        self.position -= quantity
                else:
                    self.logger.error("订单执行失败")
            
            except Exception as e:
                self.logger.error(f"下单出错: {e}")
        
        # 模拟交易模式
        else:
            if side == "BUY":
                self.position += quantity
            else:
                self.position -= quantity
            
            self.logger.info(f"当前持仓: {self.position:.4f}")
    
    def get_status(self) -> Dict[str, Any]:
        """获取策略运行状态"""
        return {
            'is_running': self.is_running,
            'position': self.position,
            'last_signal': self.last_signal,
            'symbol': self.strategy.getSymbol() if CPP_MODULE_AVAILABLE else "N/A",
            'fast_ma': self.strategy.getFastMA() if CPP_MODULE_AVAILABLE else 0.0,
            'slow_ma': self.strategy.getSlowMA() if CPP_MODULE_AVAILABLE else 0.0,
        }
    
    def _signal_to_string(self, signal) -> str:
        """将信号转换为字符串"""
        if CPP_MODULE_AVAILABLE:
            if signal == Signal.BUY:
                return "BUY"
            elif signal == Signal.SELL:
                return "SELL"
            else:
                return "HOLD"
        else:
            return str(signal)
