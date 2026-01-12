package main

import (
	"testing"
	"time"
)

func TestSideString(t *testing.T) {
	tests := []struct {
		name string
		side Side
		want string
	}{
		{"Buy side", Buy, "BUY"},
		{"Sell side", Sell, "SELL"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.side.String(); got != tt.want {
				t.Errorf("Side.String() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestOrderCreation(t *testing.T) {
	order := &Order{
		ID:        1,
		Price:     100.0,
		Quantity:  1000,
		Side:      Buy,
		EntryTime: time.Now(),
	}

	if order.ID != 1 {
		t.Errorf("Order ID = %v, want 1", order.ID)
	}
	if order.Price != 100.0 {
		t.Errorf("Order Price = %v, want 100.0", order.Price)
	}
	if order.Quantity != 1000 {
		t.Errorf("Order Quantity = %v, want 1000", order.Quantity)
	}
	if order.Side != Buy {
		t.Errorf("Order Side = %v, want Buy", order.Side)
	}
	if order.EntryTime.IsZero() {
		t.Error("Order EntryTime should not be zero")
	}
}

func TestLimitLevelCreation(t *testing.T) {
	level := &LimitLevel{
		Price:       100.0,
		TotalVolume: 5000,
		Orders:      []*Order{},
	}

	if level.Price != 100.0 {
		t.Errorf("LimitLevel Price = %v, want 100.0", level.Price)
	}
	if level.TotalVolume != 5000 {
		t.Errorf("LimitLevel TotalVolume = %v, want 5000", level.TotalVolume)
	}
	if len(level.Orders) != 0 {
		t.Errorf("LimitLevel Orders length = %v, want 0", len(level.Orders))
	}
}
