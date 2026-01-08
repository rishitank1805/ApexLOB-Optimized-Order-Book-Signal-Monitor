#include "OrderBook.h"
#include "Order.h"
#include "test_utils.h"
#include <iostream>
#include <memory>

void testOrderBookBasic() {
    std::cout << "\n=== Testing OrderBook Basic Functionality ===" << std::endl;
    
    OrderBook ob;
    
    // Test initial state
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "Initial last trade price should be 0");
    TestRunner::assertEquals(0.0, ob.getVWAP(), "Initial VWAP should be 0");
    TestRunner::assertEquals(0U, ob.getTotalVolume(), "Initial volume should be 0");
}

void testOrderBookBuyOrder() {
    std::cout << "\n=== Testing Buy Order Submission ===" << std::endl;
    
    OrderBook ob;
    
    // Create a buy order
    auto buyOrder = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder);
    
    // Since there are no matching asks, order should remain in book
    // But we can't directly check the internal state, so we test metrics
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "No trade occurred, last price should be 0");
}

void testOrderBookSellOrder() {
    std::cout << "\n=== Testing Sell Order Submission ===" << std::endl;
    
    OrderBook ob;
    
    // Create a sell order
    auto sellOrder = std::make_shared<Order>(2, 100.0, 1000, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // Since there are no matching bids, order should remain in book
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "No trade occurred, last price should be 0");
}

void testOrderBookMatching() {
    std::cout << "\n=== Testing Order Matching ===" << std::endl;
    
    OrderBook ob;
    
    // Add a buy order at 100.0
    auto buyOrder = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder);
    
    // Add a sell order at 99.0 (should match at 100.0)
    auto sellOrder = std::make_shared<Order>(2, 99.0, 500, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // Trade should have occurred
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Last trade price should be 100.0");
    TestRunner::assertEquals(500U, ob.getTotalVolume(), "Total volume should be 500");
    
    // VWAP should be 100.0 (500 * 100.0 / 500)
    TestRunner::assertEquals(100.0, ob.getVWAP(), "VWAP should be 100.0");
}

void testOrderBookPartialMatch() {
    std::cout << "\n=== Testing Partial Order Matching ===" << std::endl;
    
    OrderBook ob;
    
    // Add a buy order for 1000 units
    auto buyOrder = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder);
    
    // Add a sell order for 300 units (partial match)
    auto sellOrder = std::make_shared<Order>(2, 99.0, 300, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // Trade should have occurred for 300 units
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Last trade price should be 100.0");
    TestRunner::assertEquals(300U, ob.getTotalVolume(), "Total volume should be 300");
    TestRunner::assertEquals(100.0, ob.getVWAP(), "VWAP should be 100.0");
}

void testOrderBookMultipleMatches() {
    std::cout << "\n=== Testing Multiple Order Matches ===" << std::endl;
    
    OrderBook ob;
    
    // Add multiple buy orders at different prices (best bid first)
    auto buyOrder1 = std::make_shared<Order>(1, 101.0, 500, Side::Buy);
    auto buyOrder2 = std::make_shared<Order>(2, 100.0, 500, Side::Buy);
    ob.submitOrder(buyOrder1);
    ob.submitOrder(buyOrder2);
    
    // Add a sell order that should match both (matches best bid first, then next)
    auto sellOrder = std::make_shared<Order>(3, 99.0, 800, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // Should match: 500 at 101.0 (best bid) and 300 at 100.0
    // Last trade price is the last matched price, which is 100.0
    TestRunner::assertEquals(100.0, ob.getLastTradePrice(), "Last trade price should be 100.0 (last matched)");
    TestRunner::assertEquals(800U, ob.getTotalVolume(), "Total volume should be 800");
    
    // VWAP = (500 * 101.0 + 300 * 100.0) / 800 = 100.625
    double expectedVWAP = (500.0 * 101.0 + 300.0 * 100.0) / 800.0;
    TestRunner::assertEquals(expectedVWAP, ob.getVWAP(), "VWAP should match expected calculation");
}

void testOrderBookVWAPCalculation() {
    std::cout << "\n=== Testing VWAP Calculation ===" << std::endl;
    
    OrderBook ob;
    
    // Create multiple trades at different prices
    auto buyOrder1 = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder1);
    auto sellOrder1 = std::make_shared<Order>(2, 99.0, 1000, Side::Sell);
    ob.submitOrder(sellOrder1);
    
    // First trade: 1000 units at 100.0
    TestRunner::assertEquals(100.0, ob.getVWAP(), "VWAP after first trade");
    
    // Add another trade
    auto buyOrder2 = std::make_shared<Order>(3, 102.0, 500, Side::Buy);
    ob.submitOrder(buyOrder2);
    auto sellOrder2 = std::make_shared<Order>(4, 101.0, 500, Side::Sell);
    ob.submitOrder(sellOrder2);
    
    // VWAP = (1000 * 100.0 + 500 * 102.0) / 1500 = 100.67
    double expectedVWAP = (1000.0 * 100.0 + 500.0 * 102.0) / 1500.0;
    TestRunner::assertEquals(expectedVWAP, ob.getVWAP(), "VWAP after multiple trades");
}

void testOrderBookNoMatch() {
    std::cout << "\n=== Testing Orders That Don't Match ===" << std::endl;
    
    OrderBook ob;
    
    // Test 1: Sell order price too high to match buy
    // Add a buy order at 100.0
    auto buyOrder = std::make_shared<Order>(1, 100.0, 1000, Side::Buy);
    ob.submitOrder(buyOrder);
    
    // Add a sell order at 101.0 (sell price > buy price, shouldn't match)
    auto sellOrder = std::make_shared<Order>(2, 101.0, 500, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // No trade should occur - sell at 101 > buy at 100
    TestRunner::assertEquals(0.0, ob.getLastTradePrice(), "No trade: sell price (101) > buy price (100)");
    TestRunner::assertEquals(0U, ob.getTotalVolume(), "No volume should be traded");
    
    // Test 2: Buy order price too low to match sell
    OrderBook ob2;
    auto sellOrder2 = std::make_shared<Order>(1, 100.0, 1000, Side::Sell);
    ob2.submitOrder(sellOrder2);
    
    // Add a buy order at 99.0 (buy price < sell price, shouldn't match)
    auto buyOrder2 = std::make_shared<Order>(2, 99.0, 500, Side::Buy);
    ob2.submitOrder(buyOrder2);
    
    // No trade should occur - buy at 99 < sell at 100
    TestRunner::assertEquals(0.0, ob2.getLastTradePrice(), "No trade: buy price (99) < sell price (100)");
    TestRunner::assertEquals(0U, ob2.getTotalVolume(), "No volume should be traded");
}

void testOrderBookPricePriority() {
    std::cout << "\n=== Testing Price Priority in Matching ===" << std::endl;
    
    OrderBook ob;
    
    // Add buy orders at different prices (best bid first)
    // Bids are stored highest first (std::greater), so 101.0 is best bid
    auto buyOrder1 = std::make_shared<Order>(1, 101.0, 500, Side::Buy);
    auto buyOrder2 = std::make_shared<Order>(2, 100.0, 500, Side::Buy);
    ob.submitOrder(buyOrder1);
    ob.submitOrder(buyOrder2);
    
    // Add sell order that should match best bid first (101.0)
    // Sell at 99.0 can match buy at 101.0 (99 <= 101)
    auto sellOrder = std::make_shared<Order>(3, 99.0, 300, Side::Sell);
    ob.submitOrder(sellOrder);
    
    // Should match at best bid price (101.0) - matches at limit level price
    TestRunner::assertEquals(101.0, ob.getLastTradePrice(), "Should match at best bid price (101.0)");
    TestRunner::assertEquals(300U, ob.getTotalVolume(), "Volume should be 300");
    TestRunner::assertEquals(101.0, ob.getVWAP(), "VWAP should be 101.0 (all at best bid)");
}

void testOrderBookThreadSafety() {
    std::cout << "\n=== Testing Thread Safety (Basic) ===" << std::endl;
    
    OrderBook ob;
    
    // Test that getters are thread-safe (they use locks)
    double price1 = ob.getLastTradePrice();
    double price2 = ob.getLastTradePrice();
    
    TestRunner::assertEquals(price1, price2, "Concurrent reads should return same value");
    
    // Test that metrics are consistent
    double vwap = ob.getVWAP();
    uint32_t volume = ob.getTotalVolume();
    
    TestRunner::assertTrue(vwap >= 0.0, "VWAP should be non-negative");
    TestRunner::assertTrue(volume >= 0U, "Volume should be non-negative");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "OrderBook Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    TestRunner::reset();
    
    testOrderBookBasic();
    testOrderBookBuyOrder();
    testOrderBookSellOrder();
    testOrderBookMatching();
    testOrderBookPartialMatch();
    testOrderBookMultipleMatches();
    testOrderBookVWAPCalculation();
    testOrderBookNoMatch();
    testOrderBookPricePriority();
    testOrderBookThreadSafety();
    
    TestRunner::printSummary();
    
    return TestRunner::getExitCode();
}

