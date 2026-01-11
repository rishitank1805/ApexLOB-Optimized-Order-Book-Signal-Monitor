package main

import "time"

type Side int

const (
	Buy Side = iota
	Sell
)

func (s Side) String() string {
	switch s {
	case Buy:
		return "BUY"
	case Sell:
		return "SELL"
	default:
		return "UNKNOWN"
	}
}

type Order struct {
	ID        uint64
	Price     float64
	Quantity  uint32
	Side      Side
	EntryTime time.Time
}

type LimitLevel struct {
	Price       float64
	TotalVolume uint32
	Orders      []*Order
}
