"""
主程序入口

启动量化交易机器人
"""

import os
import sys
import yaml
import logging
import signal
from pathlib import Path

# 添加项目根目录到 Python 路径
sys.path.insert(0, str(Path(__file__).parent.parent))

# 导入模块
try:
    from app import fastquant_cpp
    from app.binance_connector import BinanceConnector
    from app.strategy_runner import StrategyRunner
    from app.pnl_tracker import PnLTracker
    CPP_MODULE_AVAILABLE = True
except ImportError as e:
    print(f"警告: 无法导入 C++ 模块: {e}")
    print("请先编译 C++ 核心库: ./scripts/build.sh")
    CPP_MODULE_AVAILABLE = False
    sys.exit(1)


class TradingBot:
    """量化交易机器人主类"""
    
    def __init__(self, config_path: str):
        """初始化交易机器人"""
        self.config = self.load_config(config_path)
        self.setup_logging()
        self.logger = logging.getLogger(__name__)
        
        # 初始化组件
        self.connector = None
        self.strategy = None
        self.runner = None
        self.pnl_tracker = PnLTracker()
        
        # 优雅退出标志
        self.should_exit = False
        signal.signal(signal.SIGINT, self.handle_exit)
        signal.signal(signal.SIGTERM, self.handle_exit)
    
    def load_config(self, config_path: str) -> dict:
        """加载配置文件"""
        if not os.path.exists(config_path):
            print(f"错误: 配置文件不存在: {config_path}")
            print("请复制 config/config.example.yaml 为 config/config.yaml")
            sys.exit(1)
        
        with open(config_path, 'r') as f:
            config = yaml.safe_load(f)
        
        return config
    
    def setup_logging(self):
        """配置日志系统"""
        log_config = self.config.get('logging', {})
        log_level = log_config.get('level', 'INFO')
        log_file = log_config.get('file', 'logs/trading.log')
        
        # 创建日志目录
        os.makedirs(os.path.dirname(log_file), exist_ok=True)
        
        # 配置日志格式
        logging.basicConfig(
            level=getattr(logging, log_level),
            format='%(asctime)s [%(levelname)s] %(name)s: %(message)s',
            handlers=[
                logging.FileHandler(log_file),
                logging.StreamHandler(sys.stdout)
            ]
        )
    
    def initialize(self):
        """初始化交易系统"""
        self.logger.info("="*60)
        self.logger.info("FastQuant 量化交易系统启动")
        self.logger.info("="*60)
        
        # 1. 连接交易所
        binance_config = self.config['binance']
        self.connector = BinanceConnector(
            api_key=binance_config['api_key'],
            api_secret=binance_config['api_secret'],
            testnet=binance_config.get('testnet', True)
        )
        
        # 2. 创建策略
        strategy_config = self.config['strategy']
        symbol = strategy_config['symbol']
        fast_period = strategy_config['fast_period']
        slow_period = strategy_config['slow_period']
        
        self.strategy = fastquant_cpp.DualMAStrategy(
            symbol, fast_period, slow_period
        )
        
        self.logger.info(f"策略: 双均线 ({fast_period}/{slow_period})")
        self.logger.info(f"交易对: {symbol}")
        
        # 3. 创建策略运行器
        trading_config = self.config['trading']
        self.runner = StrategyRunner(
            strategy=self.strategy,
            connector=self.connector,
            config=trading_config
        )
        
        # 4. 显示账户信息（如果可用）
        if not binance_config.get('testnet', True):
            account = self.connector.get_account_info()
            if account:
                self.logger.info(f"账户余额: {len(account.get('balances', []))} 种币")
        
        self.logger.info("初始化完成")
        self.logger.info("-"*60)
    
    def run(self):
        """运行交易机器人"""
        try:
            symbol = self.config['strategy']['symbol']
            
            self.logger.info(f"开始监控 {symbol}...")
            self.logger.info("按 Ctrl+C 停止运行")
            self.logger.info("-"*60)
            
            # 启动策略
            self.runner.start(symbol)
            
            # 主循环
            import time
            while not self.should_exit:
                time.sleep(1)
                
                # 定期输出状态（每 60 秒）
                if int(time.time()) % 60 == 0:
                    status = self.runner.get_status()
                    self.logger.info(f"运行状态: {status}")
        
        except Exception as e:
            self.logger.error(f"运行时错误: {e}", exc_info=True)
        
        finally:
            self.shutdown()
    
    def shutdown(self):
        """关闭系统"""
        self.logger.info("-"*60)
        self.logger.info("正在关闭...")
        
        # 停止策略
        if self.runner:
            self.runner.stop()
        
        # 断开连接
        if self.connector:
            self.connector.stop_websocket()
        
        # 输出统计
        current_price = self.connector.get_current_price(
            self.config['strategy']['symbol']
        ) if self.connector else None
        
        self.pnl_tracker.print_report(current_price)
        
        self.logger.info("系统已关闭")
        self.logger.info("="*60)
    
    def handle_exit(self, signum, frame):
        """处理退出信号"""
        self.logger.info("收到退出信号...")
        self.should_exit = True


def main():
    """主函数"""
    # 获取配置文件路径
    config_path = os.environ.get('CONFIG_FILE', 'config/config.yaml')
    
    if not os.path.exists(config_path):
        print("错误: 配置文件不存在")
        print(f"期望路径: {config_path}")
        print("\n请按以下步骤操作:")
        print("1. 复制配置文件: cp config/config.example.yaml config/config.yaml")
        print("2. 编辑配置文件，填入你的 Binance API 密钥")
        print("3. 重新运行程序")
        return 1
    
    # 创建并运行机器人
    bot = TradingBot(config_path)
    bot.initialize()
    bot.run()
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
