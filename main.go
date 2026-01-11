package main

import (
	"encoding/json"
	"fmt"
	"log"
	"os"
	"os/signal"
	"strconv"
	"sync"
	"syscall"
	"time"

	"github.com/gorilla/websocket"
)

type TimingStats struct {
	connectionStart       time.Time
	firstMessageTime      time.Time
	firstMessageReceived  bool
	totalMessages         int
	totalProcessingTimeMs float64
	mu                    sync.Mutex
}

type BinanceTrade struct {
	Price    string `json:"p"`
	Quantity string `json:"q"`
	IsMaker  bool   `json:"m"` // isBuyerMaker
	TradeID  uint64 `json:"a"`
}

var timingStats = &TimingStats{
	connectionStart: time.Now(),
}

func main() {
	ob := NewOrderBook()
	symbol := "btcusdt"
	url := fmt.Sprintf("wss://stream.binance.com:443/ws/%s@aggTrade", symbol)

	fmt.Printf("Connecting to Binance %s/USDT Live Feed...\n", symbol)
	fmt.Printf("WebSocket URL: %s\n", url)
	fmt.Println()

	// Setup graceful shutdown
	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt, syscall.SIGTERM)

	// Connect to WebSocket
	dialer := websocket.Dialer{}
	conn, _, err := dialer.Dial(url, nil)
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	defer conn.Close()

	connectionTime := time.Since(timingStats.connectionStart)
	fmt.Printf("[INFO] Connected to Binance WebSocket\n")
	fmt.Printf("[INFO] Connection established in %dms\n", connectionTime.Milliseconds())

	// Channel for messages
	done := make(chan struct{})

	go func() {
		defer close(done)
		for {
			_, message, err := conn.ReadMessage()
			if err != nil {
				if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
					log.Printf("WebSocket error: %v", err)
				}
				return
			}

			msgStart := time.Now()

			// Record first message time
			timingStats.mu.Lock()
			if !timingStats.firstMessageReceived {
				timingStats.firstMessageReceived = true
				timingStats.firstMessageTime = msgStart
				connectionTime := time.Since(timingStats.connectionStart)
				fmt.Printf("\n[INFO] First message received in %dms\n", connectionTime.Milliseconds())
			}
			timingStats.mu.Unlock()

			var trade BinanceTrade
			if err := json.Unmarshal(message, &trade); err != nil {
				log.Printf("[ERROR] JSON parse error: %v", err)
				continue
			}

			// Validate required fields
			if trade.Price == "" || trade.Quantity == "" {
				log.Printf("[WARNING] Missing required fields in message")
				continue
			}

			price, err := strconv.ParseFloat(trade.Price, 64)
			if err != nil {
				log.Printf("[ERROR] Invalid price: %v", err)
				continue
			}

			quantity, err := strconv.ParseFloat(trade.Quantity, 64)
			if err != nil {
				log.Printf("[ERROR] Invalid quantity: %v", err)
				continue
			}

			// Create order
			order := &Order{
				ID:        trade.TradeID,
				Price:     price,
				Quantity:  uint32(quantity * 1000), // Scale for integer qty
				Side:      Sell,
				EntryTime: time.Now(),
			}

			if !trade.IsMaker {
				order.Side = Buy
			}

			// Submit order
			ob.SubmitOrder(order)

			// Calculate processing time
			msgEnd := time.Now()
			processingTimeMs := float64(msgEnd.Sub(msgStart).Nanoseconds()) / 1e6

			// Update timing statistics
			timingStats.mu.Lock()
			timingStats.totalMessages++
			timingStats.totalProcessingTimeMs += processingTimeMs
			currentTotal := timingStats.totalMessages
			currentTotalTime := timingStats.totalProcessingTimeMs
			timingStats.mu.Unlock()

			// Display metrics
			ob.DisplayMetrics(currentTotal, currentTotalTime)
		}
	}()

	// Wait for interrupt or connection close
	select {
	case <-done:
		fmt.Println("\n[INFO] WebSocket connection closed")
	case <-interrupt:
		fmt.Println("\n[INFO] Interrupted by user")
	}

	// Print final statistics
	timingStats.mu.Lock()
	duration := time.Since(timingStats.connectionStart).Seconds()
	totalMsgs := timingStats.totalMessages
	totalTime := timingStats.totalProcessingTimeMs
	timingStats.mu.Unlock()

	msgsPerSec := 0.0
	avgProcTime := 0.0
	if duration > 0 {
		msgsPerSec = float64(totalMsgs) / duration
	}
	if totalMsgs > 0 {
		avgProcTime = totalTime / float64(totalMsgs)
	}

	fmt.Printf("[INFO] Connection duration: %.2f seconds\n", duration)
	fmt.Printf("[INFO] Total messages processed: %d\n", totalMsgs)
	fmt.Printf("[INFO] Messages per second: %.2f\n", msgsPerSec)
	fmt.Printf("[INFO] Average processing time: %.3f ms\n", avgProcTime)
}
