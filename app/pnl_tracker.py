"""
盈亏追踪器

记录和统计交易盈亏
"""

import logging
from typing import List, Dict, Any
from datetime import datetime


class Trade:
    """单笔交易记录"""
    
    def __init__(self, timestamp: int, symbol: str, side: str, 
                 price: float, quantity: float):
        self.timestamp = timestamp
        self.symbol = symbol
        self.side = side  # 'BUY' or 'SELL'
        self.price = price
        self.quantity = quantity
        self.pnl: float = 0.0  # 该笔交易的盈亏
    
    def __repr__(self):
        dt = datetime.fromtimestamp(self.timestamp / 1000)
        return (f"<Trade {self.side} {self.quantity}@{self.price} "
                f"on {dt.strftime('%Y-%m-%d %H:%M:%S')} PnL={self.pnl:.2f}>")


class PnLTracker:
    """
    盈亏追踪器
    
    功能：
    1. 记录所有交易
    2. 计算已实现盈亏
    3. 计算未实现盈亏
    4. 生成交易报告
    """
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.trades: List[Trade] = []
        self.realized_pnl: float = 0.0  # 已实现盈亏
        self.unrealized_pnl: float = 0.0  # 未实现盈亏
        
        # 当前持仓成本
        self.position_cost: float = 0.0
        self.position_quantity: float = 0.0
    
    def add_trade(self, symbol: str, side: str, price: float, 
                  quantity: float, timestamp: int = None):
        """
        记录一笔交易
        
        Args:
            symbol: 交易对
            side: 'BUY' or 'SELL'
            price: 成交价格
            quantity: 成交数量
            timestamp: 时间戳（毫秒）
        """
        if timestamp is None:
            timestamp = int(datetime.now().timestamp() * 1000)
        
        trade = Trade(timestamp, symbol, side, price, quantity)
        self.trades.append(trade)
        
        # 更新持仓和盈亏
        self._update_position(trade)
        
        self.logger.info(f"记录交易: {trade}")
    
    def _update_position(self, trade: Trade):
        """
        更新持仓和计算盈亏
        
        使用 FIFO (先进先出) 方法
        """
        if trade.side == 'BUY':
            # 买入：增加持仓成本
            self.position_cost += trade.price * trade.quantity
            self.position_quantity += trade.quantity
        
        elif trade.side == 'SELL':
            # 卖出：计算已实现盈亏
            if self.position_quantity > 0:
                avg_cost = self.position_cost / self.position_quantity
                pnl = (trade.price - avg_cost) * trade.quantity
                trade.pnl = pnl
                self.realized_pnl += pnl
                
                # 减少持仓
                self.position_quantity -= trade.quantity
                self.position_cost -= avg_cost * trade.quantity
                
                self.logger.info(
                    f"实现盈亏: {pnl:.2f} "
                    f"(卖出价 {trade.price:.2f} - 成本价 {avg_cost:.2f}) * {trade.quantity:.4f}"
                )
    
    def calculate_unrealized_pnl(self, current_price: float) -> float:
        """
        计算未实现盈亏（当前持仓的浮动盈亏）
        
        Args:
            current_price: 当前市场价格
        
        Returns:
            未实现盈亏
        """
        if self.position_quantity > 0:
            avg_cost = self.position_cost / self.position_quantity
            self.unrealized_pnl = (current_price - avg_cost) * self.position_quantity
        else:
            self.unrealized_pnl = 0.0
        
        return self.unrealized_pnl
    
    def get_total_pnl(self, current_price: float = None) -> float:
        """
        获取总盈亏（已实现 + 未实现）
        
        Args:
            current_price: 当前价格（用于计算未实现盈亏）
        
        Returns:
            总盈亏
        """
        if current_price is not None:
            self.calculate_unrealized_pnl(current_price)
        
        return self.realized_pnl + self.unrealized_pnl
    
    def get_statistics(self, current_price: float = None) -> Dict[str, Any]:
        """
        获取统计信息
        
        Returns:
            统计字典
        """
        total_trades = len(self.trades)
        buy_trades = sum(1 for t in self.trades if t.side == 'BUY')
        sell_trades = sum(1 for t in self.trades if t.side == 'SELL')
        
        # 胜率
        profitable_trades = sum(1 for t in self.trades if t.pnl > 0)
        win_rate = (profitable_trades / sell_trades * 100) if sell_trades > 0 else 0.0
        
        if current_price is not None:
            self.calculate_unrealized_pnl(current_price)
        
        return {
            'total_trades': total_trades,
            'buy_trades': buy_trades,
            'sell_trades': sell_trades,
            'realized_pnl': self.realized_pnl,
            'unrealized_pnl': self.unrealized_pnl,
            'total_pnl': self.realized_pnl + self.unrealized_pnl,
            'position_quantity': self.position_quantity,
            'position_cost': self.position_cost,
            'avg_cost': self.position_cost / self.position_quantity if self.position_quantity > 0 else 0.0,
            'win_rate': win_rate,
        }
    
    def print_report(self, current_price: float = None):
        """打印交易报告"""
        stats = self.get_statistics(current_price)
        
        print("\n" + "="*60)
        print("交易统计报告")
        print("="*60)
        print(f"总交易次数: {stats['total_trades']}")
        print(f"买入次数: {stats['buy_trades']}")
        print(f"卖出次数: {stats['sell_trades']}")
        print(f"胜率: {stats['win_rate']:.2f}%")
        print("-"*60)
        print(f"已实现盈亏: {stats['realized_pnl']:.2f} USDT")
        print(f"未实现盈亏: {stats['unrealized_pnl']:.2f} USDT")
        print(f"总盈亏: {stats['total_pnl']:.2f} USDT")
        print("-"*60)
        print(f"当前持仓: {stats['position_quantity']:.4f}")
        print(f"持仓成本: {stats['avg_cost']:.2f} USDT")
        print("="*60 + "\n")
    
    def export_to_csv(self, filename: str):
        """导出交易记录到 CSV"""
        import csv
        
        with open(filename, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['Timestamp', 'DateTime', 'Symbol', 'Side', 
                           'Price', 'Quantity', 'PnL'])
            
            for trade in self.trades:
                dt = datetime.fromtimestamp(trade.timestamp / 1000)
                writer.writerow([
                    trade.timestamp,
                    dt.strftime('%Y-%m-%d %H:%M:%S'),
                    trade.symbol,
                    trade.side,
                    trade.price,
                    trade.quantity,
                    trade.pnl
                ])
        
        self.logger.info(f"交易记录已导出到 {filename}")
