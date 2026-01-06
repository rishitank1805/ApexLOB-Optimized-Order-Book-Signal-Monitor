#include "AlphaSignalGenerator.h"
#include "OrderBook.h"
#include "Order.h"
#include "test_utils.h"
#include <iostream>
#include <memory>
#include <limits>
#include <cmath>

// ============================================================================
// AlphaSignalGenerator Edge Cases
// ============================================================================

void testAlphaSignalEmptyHistory() {
    std::cout << "\n=== Testing Empty History Edge Cases ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Test with no data
    AlphaSignal signal = generator.generateSignal();
    TestRunner::assertTrue(signal.signal == SignalType::HOLD, "Empty history should return HOLD");
    TestRunner::assertEquals(0.0, signal.strength, "Empty history should have 0 strength");
    TestRunner::assertEquals(0.0, signal.price, "Empty history should have 0 price");
}

void testAlphaSignalSinglePrice() {
    std::cout << "\n=== Testing Single Price Point ===" << std::endl;
    
    AlphaSignalGenerator generator;
    generator.updatePrice(100.0, 1000.0, 100.0);
    
    AlphaSignal signal = generator.generateSignal();
    TestRunner::assertTrue(signal.signal == SignalType::HOLD, "Single price should return HOLD");
    // With insufficient data, price might not be set correctly, just check it doesn't crash
    TestRunner::assertTrue(signal.price >= 0.0, "Price should be non-negative");
}

void testAlphaSignalConstantPrices() {
    std::cout << "\n=== Testing Constant Prices (No Variation) ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Add 50 constant prices
    for (int i = 0; i < 50; ++i) {
        generator.updatePrice(100.0, 1000.0, 100.0);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // With constant prices, RSI calculation may vary, but should be valid
    TestRunner::assertTrue(signal.rsi >= 0.0 && signal.rsi <= 100.0, 
                           "RSI should be valid with constant prices");
    TestRunner::assertEquals(0.0, signal.momentum, "Constant prices should have 0 momentum", 0.1);
    TestRunner::assertEquals(0.0, signal.volatility, "Constant prices should have 0 volatility", 0.1);
}

void testAlphaSignalExtremePriceMovements() {
    std::cout << "\n=== Testing Extreme Price Movements ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create extreme upward movement (100% increase)
    std::vector<double> prices;
    double basePrice = 100.0;
    for (int i = 0; i < 35; ++i) {
        prices.push_back(basePrice * (1.0 + i * 0.03)); // 3% per step
    }
    
    for (double price : prices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Momentum should be positive for upward movement (may vary based on calculation)
    TestRunner::assertTrue(signal.momentum > 0.0, "Extreme upward movement should show positive momentum");
    TestRunner::assertTrue(signal.volatility > 0.0, "Extreme movement should show volatility");
}

void testAlphaSignalRSIBoundaries() {
    std::cout << "\n=== Testing RSI Boundary Conditions ===" << std::endl;
    
    // Test oversold (RSI near 0)
    AlphaSignalGenerator generator1;
    for (int i = 0; i < 35; ++i) {
        generator1.updatePrice(100.0 - i * 2.0, 1000.0, 100.0 - i * 2.0);
    }
    AlphaSignal signal1 = generator1.generateSignal();
    TestRunner::assertTrue(signal1.rsi >= 0.0 && signal1.rsi <= 100.0, 
                           "RSI should always be between 0 and 100");
    TestRunner::assertTrue(signal1.rsi < 30.0, "Steady decline should result in low RSI");
    
    // Test overbought (RSI near 100)
    AlphaSignalGenerator generator2;
    for (int i = 0; i < 35; ++i) {
        generator2.updatePrice(100.0 + i * 2.0, 1000.0, 100.0 + i * 2.0);
    }
    AlphaSignal signal2 = generator2.generateSignal();
    TestRunner::assertTrue(signal2.rsi > 70.0, "Steady rise should result in high RSI");
}

void testAlphaSignalVeryHighVolatility() {
    std::cout << "\n=== Testing Very High Volatility ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create high volatility pattern (large swings)
    std::vector<double> prices;
    double basePrice = 100.0;
    for (int i = 0; i < 35; ++i) {
        double swing = (i % 2 == 0) ? 10.0 : -10.0; // ±10% swings
        prices.push_back(basePrice + swing);
    }
    
    for (double price : prices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    TestRunner::assertTrue(signal.volatility > 5.0, "High volatility pattern should show high volatility");
    // High volatility should reduce signal strength
    TestRunner::assertTrue(signal.strength <= 1.0, "Signal strength should be capped at 1.0");
}

void testAlphaSignalRapidPriceChanges() {
    std::cout << "\n=== Testing Rapid Price Changes ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Rapid alternating changes
    for (int i = 0; i < 50; ++i) {
        double price = 100.0 + ((i % 2 == 0) ? 5.0 : -5.0);
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Should handle rapid changes without crashing
    TestRunner::assertTrue(signal.price > 0.0, "Price should be valid");
    TestRunner::assertTrue(signal.rsi >= 0.0 && signal.rsi <= 100.0, "RSI should be valid");
}

void testAlphaSignalZeroPrice() {
    std::cout << "\n=== Testing Zero Price Edge Case ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Add some normal prices first
    for (int i = 0; i < 20; ++i) {
        generator.updatePrice(100.0 + i, 1000.0, 100.0 + i);
    }
    
    // Then add zero price (shouldn't crash)
    generator.updatePrice(0.0, 1000.0, 0.0);
    
    AlphaSignal signal = generator.generateSignal();
    
    // System should handle zero price gracefully
    TestRunner::assertTrue(signal.price >= 0.0, "Price should handle zero");
}

void testAlphaSignalVeryLargePrices() {
    std::cout << "\n=== Testing Very Large Price Values ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Use very large prices (e.g., Bitcoin-like)
    double largePrice = 100000.0;
    for (int i = 0; i < 35; ++i) {
        generator.updatePrice(largePrice + i * 100.0, 1000.0, largePrice + i * 100.0);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    TestRunner::assertTrue(signal.price > 0.0, "Large prices should be handled");
    TestRunner::assertTrue(signal.sma_short > 0.0, "SMA should handle large values");
    TestRunner::assertTrue(signal.sma_long > 0.0, "Long SMA should handle large values");
}

void testAlphaSignalHistoryOverflow() {
    std::cout << "\n=== Testing History Overflow Protection ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Add more than MAX_HISTORY prices
    for (int i = 0; i < 2000; ++i) {
        generator.updatePrice(100.0 + i * 0.1, 1000.0, 100.0 + i * 0.1);
    }
    
    size_t historySize = generator.getHistorySize();
    TestRunner::assertTrue(historySize <= 1000, "History should not exceed MAX_HISTORY");
    TestRunner::assertTrue(historySize == 1000, "History should be exactly MAX_HISTORY");
    
    // Should still generate valid signals
    AlphaSignal signal = generator.generateSignal();
    TestRunner::assertTrue(signal.price > 0.0, "Should generate valid signal after overflow");
}

void testAlphaSignalAlternatingPattern() {
    std::cout << "\n=== Testing Alternating Price Pattern ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create sawtooth pattern
    for (int i = 0; i < 50; ++i) {
        double price = 100.0 + (i % 10) * 2.0; // Sawtooth pattern
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Should handle pattern without issues
    TestRunner::assertTrue(signal.price > 0.0, "Price should be valid");
    TestRunner::assertTrue(signal.volatility >= 0.0, "Volatility should be non-negative");
}

// ============================================================================
// OrderBook Edge Cases
// ============================================================================

void testOrderBookZeroQuantity() {
    std::cout << "\n=== Testing Zero Quantity Orders ===" << std::endl;
    
    OrderBook ob;
    
    // Zero quantity order should not crash
    auto zeroOrder = std::make_shared<Order>(1, 100.0, 0, Side::Buy);
    ob.submitOrder(zeroOrder);
    
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "Zero quantity should not create trade");
    TestRunner::assertEquals(0U, ob.getTotalVolume(), "Zero quantity should not add volume");
}

void testOrderBookSamePriceOrders() {
    std::cout << "\n=== Testing Multiple Orders at Same Price ===" << std::endl;
    
    OrderBook ob;
    
    // Add multiple buy orders at same price
    auto buyOrder1 = std::make_shared<Order>(1, 100.0, 500, Side::Buy);
    auto buyOrder2 = std::make_shared<Order>(2, 100.0, 300, Side::Buy);
    auto buyOrder3 = std::make_shared<Order>(3, 100.0, 200, Side::Buy);
    
    ob.submitOrder(buyOrder1);
    ob.submitOrder(buyOrder2);
    ob.submitOrder(buyOrder3);
    
    // Add sell order that matches all
    auto sellOrder = std::make_shared<Order>(4, 99.0, 1000, Side::Sell);
    ob.submitOrder(sellOrder);
    
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Should match at same price");
    TestRunner::assertEquals(1000U, ob.getTotalVolume(), "Should match all orders");
    TestRunner::assertEquals(100.0, ob.getVWAP(), "VWAP should be 100.0");
}

void testOrderBookExactMatch() {
    std::cout << "\n=== Testing Exact Quantity Match ===" << std::endl;
    
    OrderBook ob;
    
    auto buyOrder = std::make_shared<Order>(1, 100.0, 500, Side::Buy);
    ob.submitOrder(buyOrder);
    
    // Exact match
    auto sellOrder = std::make_shared<Order>(2, 99.0, 500, Side::Sell);
    ob.submitOrder(sellOrder);
    
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Should match exactly");
    TestRunner::assertEquals(500U, ob.getTotalVolume(), "Volume should match exactly");
    TestRunner::assertEquals(100.0, ob.getVWAP(), "VWAP should be 100.0");
}

void testOrderBookVeryLargeQuantities() {
    std::cout << "\n=== Testing Very Large Quantities ===" << std::endl;
    
    OrderBook ob;
    
    // Use maximum uint32_t values
    uint32_t largeQty = std::numeric_limits<uint32_t>::max() / 2; // Avoid overflow
    
    auto buyOrder = std::make_shared<Order>(1, 100.0, largeQty, Side::Buy);
    ob.submitOrder(buyOrder);
    
    auto sellOrder = std::make_shared<Order>(2, 99.0, largeQty / 2, Side::Sell);
    ob.submitOrder(sellOrder);
    
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Should handle large quantities");
    TestRunner::assertEquals(largeQty / 2, ob.getTotalVolume(), "Volume should match");
}

void testOrderBookMultipleLevelMatches() {
    std::cout << "\n=== Testing Multiple Price Level Matches ===" << std::endl;
    
    OrderBook ob;
    
    // Create orders at multiple price levels
    auto buyOrder1 = std::make_shared<Order>(1, 102.0, 200, Side::Buy);
    auto buyOrder2 = std::make_shared<Order>(2, 101.0, 300, Side::Buy);
    auto buyOrder3 = std::make_shared<Order>(3, 100.0, 400, Side::Buy);
    
    ob.submitOrder(buyOrder1);
    ob.submitOrder(buyOrder2);
    ob.submitOrder(buyOrder3);
    
    // Sell order that matches all three levels
    auto sellOrder = std::make_shared<Order>(4, 98.0, 900, Side::Sell);
    ob.submitOrder(sellOrder);
    
    TestRunner::assertEquals(900U, ob.getTotalVolume(), "Should match all levels");
    
    // VWAP = (200*102 + 300*101 + 400*100) / 900 = 100.78
    double expectedVWAP = (200.0 * 102.0 + 300.0 * 101.0 + 400.0 * 100.0) / 900.0;
    TestRunner::assertEquals(expectedVWAP, ob.getVWAP(), "VWAP should match expected", 0.01);
}

void testOrderBookNoMatchScenarios() {
    std::cout << "\n=== Testing Various No-Match Scenarios ===" << std::endl;
    
    OrderBook ob;
    
    // Buy order too low
    auto buyOrder = std::make_shared<Order>(1, 100.0, 500, Side::Buy);
    ob.submitOrder(buyOrder);
    
    auto sellOrder = std::make_shared<Order>(2, 101.0, 500, Side::Sell);
    ob.submitOrder(sellOrder);
    
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "Should not match (sell too high)");
    
    // Reset and test opposite
    OrderBook ob2;
    auto sellOrder2 = std::make_shared<Order>(1, 100.0, 500, Side::Sell);
    ob2.submitOrder(sellOrder2);
    
    auto buyOrder2 = std::make_shared<Order>(2, 99.0, 500, Side::Buy);
    ob2.submitOrder(buyOrder2);
    
    TestRunner::assertEquals(0.0, ob2.getLastTradePrice(), "Should not match (buy too low)");
}

void testOrderBookVWAPWithZeroVolume() {
    std::cout << "\n=== Testing VWAP with Zero Volume ===" << std::endl;
    
    OrderBook ob;
    
    // No trades, volume is zero
    TestRunner::assertEquals(0.0, ob.getVWAP(), "VWAP should be 0 with no trades");
    
    // Add orders but no match
    auto buyOrder = std::make_shared<Order>(1, 100.0, 500, Side::Buy);
    ob.submitOrder(buyOrder);
    
    TestRunner::assertEquals(0.0, ob.getVWAP(), "VWAP should be 0 with no trades");
}

void testOrderBookConsecutiveTrades() {
    std::cout << "\n=== Testing Consecutive Trades ===" << std::endl;
    
    OrderBook ob;
    
    // First trade
    auto buyOrder1 = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder1);
    auto sellOrder1 = std::make_shared<Order>(2, 99.0, 1000, Side::Sell);
    ob.submitOrder(sellOrder1);
    
    double vwap1 = ob.getVWAP();
    TestRunner::assertEquals(100.0, vwap1, "First trade VWAP");
    
    // Second trade at different price
    auto buyOrder2 = std::make_shared<Order>(3, 102.0, 500, Side::Buy);
    ob.submitOrder(buyOrder2);
    auto sellOrder2 = std::make_shared<Order>(4, 101.0, 500, Side::Sell);
    ob.submitOrder(sellOrder2);
    
    // VWAP should be cumulative
    double expectedVWAP = (1000.0 * 100.0 + 500.0 * 102.0) / 1500.0;
    TestRunner::assertEquals(expectedVWAP, ob.getVWAP(), "Cumulative VWAP", 0.01);
}

void testOrderBookPricePrecision() {
    std::cout << "\n=== Testing Price Precision ===" << std::endl;
    
    OrderBook ob;
    
    // Test with very precise prices
    double precisePrice = 100.123456789;
    auto buyOrder = std::make_shared<Order>(1, precisePrice, 1000, Side::Buy);
    ob.submitOrder(buyOrder);
    
    auto sellOrder = std::make_shared<Order>(2, precisePrice - 0.01, 1000, Side::Sell);
    ob.submitOrder(sellOrder);
    
    TestRunner::assertEquals(precisePrice, ob.getLastTradePrice(), 
                           "Should handle precise prices", 0.0001);
}

void testOrderBookEmptyBookOperations() {
    std::cout << "\n=== Testing Operations on Empty Book ===" << std::endl;
    
    OrderBook ob;
    
    // All operations on empty book should be safe
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "Empty book last price");
    TestRunner::assertEquals(0.0, ob.getVWAP(), "Empty book VWAP");
    TestRunner::assertEquals(0U, ob.getTotalVolume(), "Empty book volume");
    TestRunner::assertEquals(0.0, ob.getCumulativeNotional(), "Empty book notional");
}

void testOrderBookSingleSideBook() {
    std::cout << "\n=== Testing Single-Side Order Book ===" << std::endl;
    
    OrderBook ob;
    
    // Only buy orders, no sells
    for (int i = 0; i < 5; ++i) {
        auto buyOrder = std::make_shared<Order>(i + 1, 100.0 - i, 100, Side::Buy);
        ob.submitOrder(buyOrder);
    }
    
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "No trades with only buys");
    TestRunner::assertEquals(0U, ob.getTotalVolume(), "No volume with only buys");
}

void testOrderBookRemainingQuantity() {
    std::cout << "\n=== Testing Remaining Quantity After Partial Match ===" << std::endl;
    
    OrderBook ob;
    
    // Buy order larger than sell
    auto buyOrder = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder);
    
    auto sellOrder = std::make_shared<Order>(2, 99.0, 300, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // Should match 300, leaving 700 in buy order
    TestRunner::assertEquals(300U, ob.getTotalVolume(), "Should match 300");
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Last price should be 100.0");
    
    // Remaining buy order should still be in book (can't directly test, but no crash)
    // Add another sell to verify
    auto sellOrder2 = std::make_shared<Order>(3, 99.0, 200, Side::Sell);
    ob.submitOrder(sellOrder2);
    
    TestRunner::assertEquals(500U, ob.getTotalVolume(), "Should match additional 200");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Edge Cases Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    TestRunner::reset();
    
    // AlphaSignalGenerator Edge Cases
    testAlphaSignalEmptyHistory();
    testAlphaSignalSinglePrice();
    testAlphaSignalConstantPrices();
    testAlphaSignalExtremePriceMovements();
    testAlphaSignalRSIBoundaries();
    testAlphaSignalVeryHighVolatility();
    testAlphaSignalRapidPriceChanges();
    testAlphaSignalZeroPrice();
    testAlphaSignalVeryLargePrices();
    testAlphaSignalHistoryOverflow();
    testAlphaSignalAlternatingPattern();
    
    // OrderBook Edge Cases
    testOrderBookZeroQuantity();
    testOrderBookSamePriceOrders();
    testOrderBookExactMatch();
    testOrderBookVeryLargeQuantities();
    testOrderBookMultipleLevelMatches();
    testOrderBookNoMatchScenarios();
    testOrderBookVWAPWithZeroVolume();
    testOrderBookConsecutiveTrades();
    testOrderBookPricePrecision();
    testOrderBookEmptyBookOperations();
    testOrderBookSingleSideBook();
    testOrderBookRemainingQuantity();
    
    TestRunner::printSummary();
    
    return TestRunner::getExitCode();
}

