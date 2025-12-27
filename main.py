#!/usr/bin/env python3
"""
Python implementation of ApexLOB - Optimized Order Book Signal Monitor
Connects to Binance WebSocket API to stream live BTC/USDT trading data
"""

import websocket
import json
import threading
import time
from datetime import datetime
from collections import OrderedDict
from enum import Enum
from dataclasses import dataclass
from typing import Optional, List
import sys


class Side(Enum):
    BUY = "Buy"
    SELL = "Sell"


@dataclass
class Order:
    id: int
    price: float
    quantity: int
    side: Side
    entry_time: float

    def __init__(self, order_id: int, price: float, quantity: int, side: Side):
        self.id = order_id
        self.price = price
        self.quantity = quantity
        self.side = side
        self.entry_time = time.perf_counter()


class LimitLevel:
    def __init__(self, price: float):
        self.price = price
        self.total_volume = 0
        self.orders: List[Order] = []


class OrderBook:
    def __init__(self):
        # Bids: descending order (highest first)
        self.bids: OrderedDict[float, LimitLevel] = OrderedDict()
        # Asks: ascending order (lowest first)
        self.asks: OrderedDict[float, LimitLevel] = OrderedDict()
        self.lock = threading.Lock()
        
        self.last_trade_price = 0.0
        self.total_volume_traded = 0
        self.cumulative_notional = 0.0
        
        # Timing statistics
        self.total_messages_processed = 0
        self.total_processing_time = 0.0

    def submit_order(self, order: Order):
        start_time = time.perf_counter()
        
        with self.lock:
            if order.side == Side.BUY:
                self._match_order(order, self.asks)
                if order.quantity > 0:
                    self._add_limit(order, self.bids)
            else:
                self._match_order(order, self.bids)
                if order.quantity > 0:
                    self._add_limit(order, self.asks)
        
        processing_time = (time.perf_counter() - start_time) * 1000  # Convert to ms
        self.total_messages_processed += 1
        self.total_processing_time += processing_time

    def _match_order(self, order: Order, opposite_side: OrderedDict):
        """Match order against opposite side of the book"""
        prices_to_remove = []
        
        for price, level in list(opposite_side.items()):
            if order.quantity == 0:
                break
                
            # Check if order can match at this price level
            can_match = (order.side == Side.BUY and order.price >= price) or \
                       (order.side == Side.SELL and order.price <= price)
            
            if not can_match:
                break
            
            # Match against orders at this level
            orders_to_remove = []
            for existing_order in level.orders:
                if order.quantity == 0:
                    break
                    
                traded_qty = min(order.quantity, existing_order.quantity)
                self.last_trade_price = price
                self.total_volume_traded += traded_qty
                self.cumulative_notional += (traded_qty * price)
                
                order.quantity -= traded_qty
                existing_order.quantity -= traded_qty
                level.total_volume -= traded_qty
                
                if existing_order.quantity == 0:
                    orders_to_remove.append(existing_order)
            
            # Remove filled orders
            for o in orders_to_remove:
                level.orders.remove(o)
            
            # Remove empty levels
            if len(level.orders) == 0:
                prices_to_remove.append(price)
        
        # Remove empty price levels
        for price in prices_to_remove:
            del opposite_side[price]

    def _add_limit(self, order: Order, side_map: OrderedDict):
        """Add order to limit order book"""
        if order.price not in side_map:
            side_map[order.price] = LimitLevel(order.price)
        
        level = side_map[order.price]
        level.total_volume += order.quantity
        level.orders.append(order)

    def display_metrics(self):
        """Display order book metrics with timing information"""
        with self.lock:
            vwap = (self.cumulative_notional / self.total_volume_traded 
                   if self.total_volume_traded > 0 else 0.0)
            
            avg_processing_time = (self.total_processing_time / self.total_messages_processed 
                                 if self.total_messages_processed > 0 else 0.0)
            
            current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
            
            print(f"\r[{current_time}] [LOB] Last: {self.last_trade_price:.2f} | "
                  f"VWAP: {vwap:.2f} | Vol: {self.total_volume_traded} | "
                  f"Msg: {self.total_messages_processed} | "
                  f"AvgProc: {avg_processing_time:.3f}ms", end='', flush=True)


class BinanceWebSocketClient:
    def __init__(self, symbol: str = "btcusdt"):
        self.symbol = symbol.lower()
        self.url = f"wss://stream.binance.com:443/ws/{self.symbol}@aggTrade"
        self.ws: Optional[websocket.WebSocketApp] = None
        self.order_book = OrderBook()
        self.connection_start_time: Optional[float] = None
        self.first_message_time: Optional[float] = None
        
    def on_message(self, ws, message):
        """Handle incoming WebSocket messages"""
        if self.first_message_time is None:
            self.first_message_time = time.perf_counter()
            connection_time = (self.first_message_time - self.connection_start_time) * 1000
            print(f"\n[INFO] First message received in {connection_time:.2f}ms")
        
        message_start = time.perf_counter()
        
        try:
            data = json.loads(message)
            
            # Validate required fields
            if not all(key in data for key in ["p", "q", "m", "a"]):
                print(f"\n[WARNING] Missing required fields in message: {data}")
                return
            
            # Parse trade data
            price = float(data["p"])
            quantity = float(data["q"])
            is_sell = bool(data["m"])
            trade_id = int(data["a"])
            
            # Create order
            order = Order(
                order_id=trade_id,
                price=price,
                quantity=int(quantity * 1000),  # Scale for integer qty
                side=Side.SELL if is_sell else Side.BUY
            )
            
            # Process order
            self.order_book.submit_order(order)
            self.order_book.display_metrics()
            
            message_time = (time.perf_counter() - message_start) * 1000
            # Optionally log per-message timing (commented out to reduce noise)
            # print(f" | MsgProc: {message_time:.3f}ms", end='')
            
        except json.JSONDecodeError as e:
            print(f"\n[ERROR] JSON decode error: {e}")
        except KeyError as e:
            print(f"\n[ERROR] Missing key in message: {e}")
        except Exception as e:
            print(f"\n[ERROR] Error processing message: {e}")

    def on_error(self, ws, error):
        """Handle WebSocket errors"""
        current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        print(f"\n[{current_time}] [ERROR] WebSocket error: {error}")

    def on_close(self, ws, close_status_code, close_msg):
        """Handle WebSocket close"""
        current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        duration = time.perf_counter() - self.connection_start_time if self.connection_start_time else 0
        msgs_per_sec = self.order_book.total_messages_processed / duration if duration > 0 else 0
        avg_proc_time = (self.order_book.total_processing_time / self.order_book.total_messages_processed 
                        if self.order_book.total_messages_processed > 0 else 0)
        
        print(f"\n[{current_time}] [INFO] WebSocket connection closed")
        print(f"[INFO] Connection duration: {duration:.2f} seconds")
        print(f"[INFO] Total messages processed: {self.order_book.total_messages_processed}")
        print(f"[INFO] Messages per second: {msgs_per_sec:.2f}")
        print(f"[INFO] Average processing time: {avg_proc_time:.3f} ms")

    def on_open(self, ws):
        """Handle WebSocket open"""
        current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        connection_time = (time.perf_counter() - self.connection_start_time) * 1000
        print(f"[{current_time}] [INFO] Connected to Binance WebSocket")
        print(f"[INFO] Connection established in {connection_time:.2f}ms")

    def start(self):
        """Start the WebSocket connection"""
        print(f"Connecting to Binance {self.symbol.upper()}/USDT Live Feed...")
        print(f"WebSocket URL: {self.url}")
        
        self.connection_start_time = time.perf_counter()
        
        self.ws = websocket.WebSocketApp(
            self.url,
            on_message=self.on_message,
            on_error=self.on_error,
            on_close=self.on_close,
            on_open=self.on_open
        )
        
        # Run forever
        self.ws.run_forever(sslopt={"cert_reqs": 0})


def main():
    """Main entry point"""
    try:
        client = BinanceWebSocketClient(symbol="btcusdt")
        client.start()
    except KeyboardInterrupt:
        print("\n[INFO] Interrupted by user. Shutting down...")
        sys.exit(0)
    except Exception as e:
        print(f"\n[FATAL] Unexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()

