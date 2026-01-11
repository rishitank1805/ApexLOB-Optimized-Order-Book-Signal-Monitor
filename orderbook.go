package main

import (
	"fmt"
	"sort"
	"sync"
)

type OrderBook struct {
	bids            map[float64]*LimitLevel
	asks            map[float64]*LimitLevel
	mu              sync.RWMutex
	lastTradePrice  float64
	totalVolume     uint32
	cumulativeNotional float64
}

func NewOrderBook() *OrderBook {
	return &OrderBook{
		bids: make(map[float64]*LimitLevel),
		asks: make(map[float64]*LimitLevel),
	}
}

func (ob *OrderBook) SubmitOrder(order *Order) {
	ob.mu.Lock()
	defer ob.mu.Unlock()

	if order.Side == Buy {
		ob.matchOrder(order, ob.asks, true)
		if order.Quantity > 0 {
			ob.addLimit(order, ob.bids)
		}
	} else {
		ob.matchOrder(order, ob.bids, false)
		if order.Quantity > 0 {
			ob.addLimit(order, ob.asks)
		}
	}
}

func (ob *OrderBook) matchOrder(order *Order, oppositeSide map[float64]*LimitLevel, isBuy bool) {
	// Get sorted prices for matching
	var prices []float64
	for price := range oppositeSide {
		prices = append(prices, price)
	}

	if isBuy {
		// For buy orders, match against asks (ascending order - lowest first)
		sort.Float64s(prices)
	} else {
		// For sell orders, match against bids (descending order - highest first)
		sort.Sort(sort.Reverse(sort.Float64Slice(prices)))
	}

	for _, price := range prices {
		if order.Quantity == 0 {
			break
		}

		level := oppositeSide[price]
		canMatch := false
		if isBuy {
			canMatch = order.Price >= price
		} else {
			canMatch = order.Price <= price
		}

		if !canMatch {
			break
		}

		// Match against orders at this level
		i := 0
		for i < len(level.Orders) && order.Quantity > 0 {
			existingOrder := level.Orders[i]
			if existingOrder.Quantity == 0 {
				i++
				continue
			}

			tradedQty := order.Quantity
			if existingOrder.Quantity < tradedQty {
				tradedQty = existingOrder.Quantity
			}

			ob.lastTradePrice = price
			ob.totalVolume += tradedQty
			ob.cumulativeNotional += float64(tradedQty) * price

			order.Quantity -= tradedQty
			existingOrder.Quantity -= tradedQty
			level.TotalVolume -= tradedQty

			if existingOrder.Quantity == 0 {
				// Remove order
				level.Orders = append(level.Orders[:i], level.Orders[i+1:]...)
				// Don't increment i, check same position again
			} else {
				i++
			}
		}

		// Remove empty level
		if len(level.Orders) == 0 {
			delete(oppositeSide, price)
		}
	}
}

func (ob *OrderBook) addLimit(order *Order, sideMap map[float64]*LimitLevel) {
	if level, exists := sideMap[order.Price]; exists {
		level.TotalVolume += order.Quantity
		level.Orders = append(level.Orders, order)
	} else {
		sideMap[order.Price] = &LimitLevel{
			Price:       order.Price,
			TotalVolume: order.Quantity,
			Orders:      []*Order{order},
		}
	}
}

func (ob *OrderBook) GetLastTradePrice() float64 {
	ob.mu.RLock()
	defer ob.mu.RUnlock()
	return ob.lastTradePrice
}

func (ob *OrderBook) GetVWAP() float64 {
	ob.mu.RLock()
	defer ob.mu.RUnlock()
	if ob.totalVolume == 0 {
		return 0.0
	}
	return ob.cumulativeNotional / float64(ob.totalVolume)
}

func (ob *OrderBook) GetTotalVolume() uint32 {
	ob.mu.RLock()
	defer ob.mu.RUnlock()
	return ob.totalVolume
}

func (ob *OrderBook) GetCumulativeNotional() float64 {
	ob.mu.RLock()
	defer ob.mu.RUnlock()
	return ob.cumulativeNotional
}

func (ob *OrderBook) DisplayMetrics(totalMessages int, totalProcessingTimeMs float64) {
	ob.mu.RLock()
	vwap := ob.getVWAPLocked()
	volume := ob.totalVolume
	lastPrice := ob.lastTradePrice
	ob.mu.RUnlock()

	avgProcessingTime := 0.0
	if totalMessages > 0 {
		avgProcessingTime = totalProcessingTimeMs / float64(totalMessages)
	}

	fmt.Printf("\r[LOB] Last: %.2f | VWAP: %.2f | Vol: %d", lastPrice, vwap, volume)
	if totalMessages > 0 {
		fmt.Printf(" | Msg: %d | AvgProc: %.3fms", totalMessages, avgProcessingTime)
	}
}

func (ob *OrderBook) getVWAPLocked() float64 {
	if ob.totalVolume == 0 {
		return 0.0
	}
	return ob.cumulativeNotional / float64(ob.totalVolume)
}
