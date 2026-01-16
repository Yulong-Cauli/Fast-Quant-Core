"""
Binance 交易所连接器

负责：
1. WebSocket 实时行情订阅
2. REST API 订单管理
3. 账户信息查询
"""

import time
import logging
from typing import Callable, Optional, Dict, Any
from binance.client import Client
from binance.streams import ThreadedWebsocketManager

# 尝试导入 C++ 模块
try:
    from . import fastquant_cpp
    CPP_MODULE_AVAILABLE = True
except ImportError:
    CPP_MODULE_AVAILABLE = False
    logging.warning("C++ 模块未编译，将使用纯 Python 实现")


class BinanceConnector:
    """
    Binance 交易所连接器
    
    Examples:
        >>> connector = BinanceConnector(api_key, api_secret)
        >>> connector.subscribe_ticker('BTCUSDT', on_tick_callback)
        >>> connector.place_order('BTCUSDT', 'BUY', 0.001)
    """
    
    def __init__(self, api_key: str, api_secret: str, testnet: bool = False):
        """
        初始化 Binance 连接器
        
        Args:
            api_key: Binance API Key
            api_secret: Binance API Secret
            testnet: 是否使用测试网（默认 False）
        """
        self.logger = logging.getLogger(__name__)
        self.api_key = api_key
        self.api_secret = api_secret
        self.testnet = testnet
        
        # 初始化客户端
        if testnet:
            self.client = Client(api_key, api_secret, testnet=True)
            self.logger.info("使用 Binance 测试网")
        else:
            self.client = Client(api_key, api_secret)
            self.logger.info("使用 Binance 主网")
        
        # WebSocket 管理器
        self.ws_manager: Optional[ThreadedWebsocketManager] = None
        self.active_streams: Dict[str, Any] = {}
        
    def start_websocket(self):
        """启动 WebSocket 连接"""
        if self.ws_manager is None:
            self.ws_manager = ThreadedWebsocketManager(
                api_key=self.api_key,
                api_secret=self.api_secret
            )
            self.ws_manager.start()
            self.logger.info("WebSocket 管理器已启动")
    
    def stop_websocket(self):
        """停止 WebSocket 连接"""
        if self.ws_manager is not None:
            self.ws_manager.stop()
            self.ws_manager = None
            self.active_streams.clear()
            self.logger.info("WebSocket 管理器已停止")
    
    def subscribe_ticker(self, symbol: str, callback: Callable):
        """
        订阅实时行情 Ticker
        
        Args:
            symbol: 交易对，如 'BTCUSDT'
            callback: 回调函数，接收 Tick 数据
        """
        if self.ws_manager is None:
            self.start_websocket()
        
        def handle_socket_message(msg):
            """处理 WebSocket 消息"""
            if msg['e'] == 'error':
                self.logger.error(f"WebSocket 错误: {msg}")
                return
            
            # 解析为 Tick 对象
            if CPP_MODULE_AVAILABLE:
                tick = fastquant_cpp.Tick(
                    symbol=msg['s'],
                    price=float(msg['c']),  # 最新价格
                    volume=float(msg['v']),  # 24h 成交量
                    timestamp=int(msg['E'])  # 事件时间
                )
            else:
                # 纯 Python 实现
                tick = {
                    'symbol': msg['s'],
                    'price': float(msg['c']),
                    'volume': float(msg['v']),
                    'timestamp': int(msg['E'])
                }
            
            callback(tick)
        
        # 启动 24h Ticker 流
        stream_key = self.ws_manager.start_symbol_ticker_socket(
            callback=handle_socket_message,
            symbol=symbol.lower()
        )
        
        self.active_streams[symbol] = stream_key
        self.logger.info(f"已订阅 {symbol} 实时行情")
    
    def unsubscribe_ticker(self, symbol: str):
        """取消订阅行情"""
        if symbol in self.active_streams:
            self.ws_manager.stop_socket(self.active_streams[symbol])
            del self.active_streams[symbol]
            self.logger.info(f"已取消订阅 {symbol}")
    
    def get_account_info(self) -> Dict:
        """获取账户信息"""
        try:
            account = self.client.get_account()
            return account
        except Exception as e:
            self.logger.error(f"获取账户信息失败: {e}")
            return {}
    
    def get_current_price(self, symbol: str) -> float:
        """获取当前价格"""
        try:
            ticker = self.client.get_symbol_ticker(symbol=symbol)
            return float(ticker['price'])
        except Exception as e:
            self.logger.error(f"获取 {symbol} 价格失败: {e}")
            return 0.0
    
    def place_order(self, symbol: str, side: str, quantity: float,
                   order_type: str = 'MARKET', price: Optional[float] = None) -> Dict:
        """
        下单
        
        Args:
            symbol: 交易对
            side: 'BUY' 或 'SELL'
            quantity: 数量
            order_type: 'MARKET' 或 'LIMIT'
            price: 限价单价格（仅限价单需要）
        
        Returns:
            订单信息字典
        """
        try:
            if order_type == 'MARKET':
                order = self.client.order_market(
                    symbol=symbol,
                    side=side,
                    quantity=quantity
                )
            elif order_type == 'LIMIT':
                if price is None:
                    raise ValueError("限价单必须指定价格")
                order = self.client.order_limit(
                    symbol=symbol,
                    side=side,
                    quantity=quantity,
                    price=str(price)
                )
            else:
                raise ValueError(f"不支持的订单类型: {order_type}")
            
            self.logger.info(f"订单已下: {symbol} {side} {quantity} @ {order_type}")
            return order
        
        except Exception as e:
            self.logger.error(f"下单失败: {e}")
            return {}
    
    def get_historical_klines(self, symbol: str, interval: str, 
                             limit: int = 500) -> list:
        """
        获取历史 K 线数据
        
        Args:
            symbol: 交易对
            interval: 时间间隔，如 '1m', '5m', '1h', '1d'
            limit: 数据条数（最多 1000）
        
        Returns:
            K 线数据列表
        """
        try:
            klines = self.client.get_klines(
                symbol=symbol,
                interval=interval,
                limit=limit
            )
            return klines
        except Exception as e:
            self.logger.error(f"获取历史数据失败: {e}")
            return []
    
    def __enter__(self):
        """上下文管理器：进入"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """上下文管理器：退出"""
        self.stop_websocket()
