package main

import (
	"math"
	"testing"
)

func TestNewOrderBook(t *testing.T) {
	ob := NewOrderBook()
	if ob == nil {
		t.Fatal("NewOrderBook() returned nil")
	}
	if ob.bids == nil {
		t.Error("OrderBook bids map should not be nil")
	}
	if ob.asks == nil {
		t.Error("OrderBook asks map should not be nil")
	}
	if ob.lastTradePrice != 0.0 {
		t.Errorf("Initial lastTradePrice = %v, want 0.0", ob.lastTradePrice)
	}
	if ob.totalVolume != 0 {
		t.Errorf("Initial totalVolume = %v, want 0", ob.totalVolume)
	}
}

func TestOrderBookInitialState(t *testing.T) {
	ob := NewOrderBook()

	if ob.GetLastTradePrice() != 0.0 {
		t.Errorf("GetLastTradePrice() = %v, want 0.0", ob.GetLastTradePrice())
	}
	if ob.GetVWAP() != 0.0 {
		t.Errorf("GetVWAP() = %v, want 0.0", ob.GetVWAP())
	}
	if ob.GetTotalVolume() != 0 {
		t.Errorf("GetTotalVolume() = %v, want 0", ob.GetTotalVolume())
	}
}

func TestOrderBookBuyOrder(t *testing.T) {
	ob := NewOrderBook()
	order := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}

	ob.SubmitOrder(order)

	// Since there are no matching asks, order should remain in book
	if ob.GetLastTradePrice() != 0.0 {
		t.Errorf("GetLastTradePrice() = %v, want 0.0 (no trade occurred)", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 0 {
		t.Errorf("GetTotalVolume() = %v, want 0 (no trade occurred)", ob.GetTotalVolume())
	}
}

func TestOrderBookSellOrder(t *testing.T) {
	ob := NewOrderBook()
	order := &Order{
		ID:       2,
		Price:    100.0,
		Quantity: 1000,
		Side:     Sell,
	}

	ob.SubmitOrder(order)

	// Since there are no matching bids, order should remain in book
	if ob.GetLastTradePrice() != 0.0 {
		t.Errorf("GetLastTradePrice() = %v, want 0.0 (no trade occurred)", ob.GetLastTradePrice())
	}
}

func TestOrderBookMatching(t *testing.T) {
	ob := NewOrderBook()

	// Add a buy order at 100.0
	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	// Add a sell order at 99.0 (should match at 100.0)
	sellOrder := &Order{
		ID:       2,
		Price:    99.0,
		Quantity: 500,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Trade should have occurred
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 500 {
		t.Errorf("GetTotalVolume() = %v, want 500", ob.GetTotalVolume())
	}

	// VWAP should be 100.0 (500 * 100.0 / 500)
	vwap := ob.GetVWAP()
	if math.Abs(vwap-100.0) > 0.01 {
		t.Errorf("GetVWAP() = %v, want 100.0", vwap)
	}
}

func TestOrderBookPartialMatch(t *testing.T) {
	ob := NewOrderBook()

	// Add a buy order for 1000 units
	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	// Add a sell order for 300 units (partial match)
	sellOrder := &Order{
		ID:       2,
		Price:    99.0,
		Quantity: 300,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Trade should have occurred for 300 units
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 300 {
		t.Errorf("GetTotalVolume() = %v, want 300", ob.GetTotalVolume())
	}
	if math.Abs(ob.GetVWAP()-100.0) > 0.01 {
		t.Errorf("GetVWAP() = %v, want 100.0", ob.GetVWAP())
	}
}

func TestOrderBookMultipleMatches(t *testing.T) {
	ob := NewOrderBook()

	// Add multiple buy orders at different prices (best bid first)
	buyOrder1 := &Order{
		ID:       1,
		Price:    101.0,
		Quantity: 500,
		Side:     Buy,
	}
	buyOrder2 := &Order{
		ID:       2,
		Price:    100.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder1)
	ob.SubmitOrder(buyOrder2)

	// Add a sell order that should match both
	sellOrder := &Order{
		ID:       3,
		Price:    99.0,
		Quantity: 800,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Should match: 500 at 101.0 and 300 at 100.0
	// Last trade price is the last matched price, which is 100.0
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0 (last matched)", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 800 {
		t.Errorf("GetTotalVolume() = %v, want 800", ob.GetTotalVolume())
	}

	// VWAP = (500 * 101.0 + 300 * 100.0) / 800 = 100.625
	expectedVWAP := (500.0*101.0 + 300.0*100.0) / 800.0
	vwap := ob.GetVWAP()
	if math.Abs(vwap-expectedVWAP) > 0.01 {
		t.Errorf("GetVWAP() = %v, want %v", vwap, expectedVWAP)
	}
}

func TestOrderBookVWAPCalculation(t *testing.T) {
	ob := NewOrderBook()

	// Create multiple trades at different prices
	buyOrder1 := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder1)
	sellOrder1 := &Order{
		ID:       2,
		Price:    99.0,
		Quantity: 1000,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder1)

	// First trade: 1000 units at 100.0
	vwap1 := ob.GetVWAP()
	if math.Abs(vwap1-100.0) > 0.01 {
		t.Errorf("VWAP after first trade = %v, want 100.0", vwap1)
	}

	// Add another trade
	buyOrder2 := &Order{
		ID:       3,
		Price:    102.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder2)
	sellOrder2 := &Order{
		ID:       4,
		Price:    101.0,
		Quantity: 500,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder2)

	// VWAP = (1000 * 100.0 + 500 * 102.0) / 1500 = 100.67
	expectedVWAP := (1000.0*100.0 + 500.0*102.0) / 1500.0
	vwap2 := ob.GetVWAP()
	if math.Abs(vwap2-expectedVWAP) > 0.01 {
		t.Errorf("VWAP after multiple trades = %v, want %v", vwap2, expectedVWAP)
	}
}

func TestOrderBookNoMatch(t *testing.T) {
	ob := NewOrderBook()

	// Add a buy order at 100.0
	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	// Add a sell order at 101.0 (too high, shouldn't match)
	sellOrder := &Order{
		ID:       2,
		Price:    101.0,
		Quantity: 500,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// No trade should occur
	if ob.GetLastTradePrice() != 0.0 {
		t.Errorf("GetLastTradePrice() = %v, want 0.0 (no trade)", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 0 {
		t.Errorf("GetTotalVolume() = %v, want 0 (no trade)", ob.GetTotalVolume())
	}
}

func TestOrderBookPricePriority(t *testing.T) {
	ob := NewOrderBook()

	// Add buy orders at different prices (best bid first)
	buyOrder1 := &Order{
		ID:       1,
		Price:    101.0,
		Quantity: 500,
		Side:     Buy,
	}
	buyOrder2 := &Order{
		ID:       2,
		Price:    100.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder1)
	ob.SubmitOrder(buyOrder2)

	// Add sell order that should match best bid first
	sellOrder := &Order{
		ID:       3,
		Price:    99.0,
		Quantity: 300,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Should match at best bid price (101.0)
	if ob.GetLastTradePrice() != 101.0 {
		t.Errorf("GetLastTradePrice() = %v, want 101.0 (best bid)", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 300 {
		t.Errorf("GetTotalVolume() = %v, want 300", ob.GetTotalVolume())
	}
	if math.Abs(ob.GetVWAP()-101.0) > 0.01 {
		t.Errorf("GetVWAP() = %v, want 101.0", ob.GetVWAP())
	}
}

func TestOrderBookExactMatch(t *testing.T) {
	ob := NewOrderBook()

	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	// Exact match
	sellOrder := &Order{
		ID:       2,
		Price:    99.0,
		Quantity: 500,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Should match exactly
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 500 {
		t.Errorf("GetTotalVolume() = %v, want 500", ob.GetTotalVolume())
	}
	if math.Abs(ob.GetVWAP()-100.0) > 0.01 {
		t.Errorf("GetVWAP() = %v, want 100.0", ob.GetVWAP())
	}
}

func TestOrderBookSamePriceOrders(t *testing.T) {
	ob := NewOrderBook()

	// Add multiple buy orders at same price
	buyOrder1 := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 500,
		Side:     Buy,
	}
	buyOrder2 := &Order{
		ID:       2,
		Price:    100.0,
		Quantity: 300,
		Side:     Buy,
	}
	buyOrder3 := &Order{
		ID:       3,
		Price:    100.0,
		Quantity: 200,
		Side:     Buy,
	}

	ob.SubmitOrder(buyOrder1)
	ob.SubmitOrder(buyOrder2)
	ob.SubmitOrder(buyOrder3)

	// Add sell order that matches all
	sellOrder := &Order{
		ID:       4,
		Price:    99.0,
		Quantity: 1000,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Should match at same price
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 1000 {
		t.Errorf("GetTotalVolume() = %v, want 1000", ob.GetTotalVolume())
	}
	if math.Abs(ob.GetVWAP()-100.0) > 0.01 {
		t.Errorf("GetVWAP() = %v, want 100.0", ob.GetVWAP())
	}
}

func TestOrderBookZeroQuantity(t *testing.T) {
	ob := NewOrderBook()

	// Zero quantity order should not crash
	zeroOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 0,
		Side:     Buy,
	}
	ob.SubmitOrder(zeroOrder)

	if ob.GetLastTradePrice() != 0.0 {
		t.Errorf("GetLastTradePrice() = %v, want 0.0 (zero quantity)", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 0 {
		t.Errorf("GetTotalVolume() = %v, want 0 (zero quantity)", ob.GetTotalVolume())
	}
}

func TestOrderBookVWAPWithZeroVolume(t *testing.T) {
	ob := NewOrderBook()

	// No trades, volume is zero
	vwap := ob.GetVWAP()
	if vwap != 0.0 {
		t.Errorf("GetVWAP() = %v, want 0.0 (no trades)", vwap)
	}

	// Add orders but no match
	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	vwap2 := ob.GetVWAP()
	if vwap2 != 0.0 {
		t.Errorf("GetVWAP() = %v, want 0.0 (no trades)", vwap2)
	}
}

func TestOrderBookConsecutiveTrades(t *testing.T) {
	ob := NewOrderBook()

	// First trade
	buyOrder1 := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder1)
	sellOrder1 := &Order{
		ID:       2,
		Price:    99.0,
		Quantity: 1000,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder1)

	vwap1 := ob.GetVWAP()
	if math.Abs(vwap1-100.0) > 0.01 {
		t.Errorf("First trade VWAP = %v, want 100.0", vwap1)
	}

	// Second trade at different price
	buyOrder2 := &Order{
		ID:       3,
		Price:    102.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder2)
	sellOrder2 := &Order{
		ID:       4,
		Price:    101.0,
		Quantity: 500,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder2)

	// VWAP should be cumulative
	expectedVWAP := (1000.0*100.0 + 500.0*102.0) / 1500.0
	vwap2 := ob.GetVWAP()
	if math.Abs(vwap2-expectedVWAP) > 0.01 {
		t.Errorf("Cumulative VWAP = %v, want %v", vwap2, expectedVWAP)
	}
}

func TestOrderBookRemainingQuantity(t *testing.T) {
	ob := NewOrderBook()

	// Buy order larger than sell
	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 1000,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	// First sell order - partial match
	sellOrder1 := &Order{
		ID:       2,
		Price:    99.0,
		Quantity: 300,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder1)

	// Should match 300, leaving 700 in buy order
	if ob.GetTotalVolume() != 300 {
		t.Errorf("First match: GetTotalVolume() = %v, want 300", ob.GetTotalVolume())
	}
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0", ob.GetLastTradePrice())
	}

	// Remaining buy order should still be in book - verify by matching more
	sellOrder2 := &Order{
		ID:       3,
		Price:    99.0,
		Quantity: 200,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder2)

	// Should match additional 200 from remaining buy order
	if ob.GetTotalVolume() != 500 {
		t.Errorf("Total after second match: GetTotalVolume() = %v, want 500", ob.GetTotalVolume())
	}
}

func TestOrderBookConcurrentAccess(t *testing.T) {
	ob := NewOrderBook()
	done := make(chan bool)

	// Test concurrent reads
	go func() {
		for i := 0; i < 100; i++ {
			_ = ob.GetLastTradePrice()
			_ = ob.GetVWAP()
			_ = ob.GetTotalVolume()
		}
		done <- true
	}()

	go func() {
		for i := 0; i < 100; i++ {
			order := &Order{
				ID:       uint64(i),
				Price:    100.0 + float64(i),
				Quantity: 100,
				Side:     Buy,
			}
			ob.SubmitOrder(order)
		}
		done <- true
	}()

	// Wait for both goroutines
	<-done
	<-done

	// Should not have crashed
	if ob == nil {
		t.Fatal("OrderBook should not be nil after concurrent access")
	}
}

func TestOrderBookEmptyBookOperations(t *testing.T) {
	ob := NewOrderBook()

	// All operations on empty book should be safe
	if ob.GetLastTradePrice() != 0.0 {
		t.Errorf("Empty book GetLastTradePrice() = %v, want 0.0", ob.GetLastTradePrice())
	}
	if ob.GetVWAP() != 0.0 {
		t.Errorf("Empty book GetVWAP() = %v, want 0.0", ob.GetVWAP())
	}
	if ob.GetTotalVolume() != 0 {
		t.Errorf("Empty book GetTotalVolume() = %v, want 0", ob.GetTotalVolume())
	}
	if ob.GetCumulativeNotional() != 0.0 {
		t.Errorf("Empty book GetCumulativeNotional() = %v, want 0.0", ob.GetCumulativeNotional())
	}
}

func TestOrderBookEqualPriceMatch(t *testing.T) {
	ob := NewOrderBook()

	// Buy order at 100.0
	buyOrder := &Order{
		ID:       1,
		Price:    100.0,
		Quantity: 500,
		Side:     Buy,
	}
	ob.SubmitOrder(buyOrder)

	// Sell order at 100.0 (equal prices should match)
	sellOrder := &Order{
		ID:       2,
		Price:    100.0,
		Quantity: 500,
		Side:     Sell,
	}
	ob.SubmitOrder(sellOrder)

	// Should match at equal prices
	if ob.GetLastTradePrice() != 100.0 {
		t.Errorf("GetLastTradePrice() = %v, want 100.0 (equal prices)", ob.GetLastTradePrice())
	}
	if ob.GetTotalVolume() != 500 {
		t.Errorf("GetTotalVolume() = %v, want 500", ob.GetTotalVolume())
	}
}
