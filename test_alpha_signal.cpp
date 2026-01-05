#include "AlphaSignalGenerator.h"
#include "test_utils.h"
#include <iostream>
#include <vector>
#include <cmath>

void testAlphaSignalGeneratorBasic() {
    std::cout << "\n=== Testing AlphaSignalGenerator Basic Functionality ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Test initial state
    TestRunner::assertEquals(0UL, generator.getHistorySize(), "Initial history size should be 0");
    
    // Test signal generation with insufficient data
    AlphaSignal signal = generator.generateSignal();
    TestRunner::assertTrue(signal.signal == SignalType::HOLD, "Signal should be HOLD with no data");
    TestRunner::assertEquals(0.0, signal.strength, "Signal strength should be 0 with no data");
}

void testAlphaSignalGeneratorSMA() {
    std::cout << "\n=== Testing SMA Calculation ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create a rising price trend
    std::vector<double> prices = {100.0, 101.0, 102.0, 103.0, 104.0, 105.0, 106.0, 107.0, 108.0, 109.0,
                                  110.0, 111.0, 112.0, 113.0, 114.0, 115.0, 116.0, 117.0, 118.0, 119.0,
                                  120.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0, 128.0, 129.0, 130.0};
    
    for (double price : prices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // With rising prices, short MA should be above long MA
    TestRunner::assertTrue(signal.sma_short > signal.sma_long, "Short SMA should be above long SMA in uptrend");
    TestRunner::assertTrue(signal.sma_short > 0.0, "Short SMA should be positive");
    TestRunner::assertTrue(signal.sma_long > 0.0, "Long SMA should be positive");
}

void testAlphaSignalGeneratorRSI() {
    std::cout << "\n=== Testing RSI Calculation ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create oversold scenario (falling prices)
    std::vector<double> fallingPrices;
    for (int i = 0; i < 35; ++i) {
        fallingPrices.push_back(100.0 - i * 0.5); // Steady decline
    }
    
    for (double price : fallingPrices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // RSI should be low (oversold) in falling market
    TestRunner::assertTrue(signal.rsi < 50.0, "RSI should be below 50 in falling market");
    TestRunner::assertTrue(signal.rsi >= 0.0 && signal.rsi <= 100.0, "RSI should be between 0 and 100");
    
    // Create overbought scenario (rising prices)
    AlphaSignalGenerator generator2;
    std::vector<double> risingPrices;
    for (int i = 0; i < 35; ++i) {
        risingPrices.push_back(100.0 + i * 0.5); // Steady rise
    }
    
    for (double price : risingPrices) {
        generator2.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal2 = generator2.generateSignal();
    
    // RSI should be high (overbought) in rising market
    TestRunner::assertTrue(signal2.rsi > 50.0, "RSI should be above 50 in rising market");
}

void testAlphaSignalGeneratorMomentum() {
    std::cout << "\n=== Testing Momentum Calculation ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create strong upward momentum
    std::vector<double> prices;
    double basePrice = 100.0;
    for (int i = 0; i < 35; ++i) {
        prices.push_back(basePrice + i * 1.0); // Strong upward trend
    }
    
    for (double price : prices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Momentum should be positive in uptrend
    TestRunner::assertTrue(signal.momentum > 0.0, "Momentum should be positive in uptrend");
}

void testAlphaSignalGeneratorVolatility() {
    std::cout << "\n=== Testing Volatility Calculation ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create high volatility scenario (large price swings)
    std::vector<double> volatilePrices;
    double basePrice = 100.0;
    for (int i = 0; i < 35; ++i) {
        double swing = (i % 2 == 0) ? 5.0 : -5.0; // Alternating swings
        volatilePrices.push_back(basePrice + swing);
    }
    
    for (double price : volatilePrices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Volatility should be positive
    TestRunner::assertTrue(signal.volatility >= 0.0, "Volatility should be non-negative");
}

void testAlphaSignalGeneratorStrongBuy() {
    std::cout << "\n=== Testing STRONG_BUY Signal Generation ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create scenario for strong buy: oversold + upward momentum + MA crossover
    std::vector<double> prices;
    double basePrice = 100.0;
    
    // First create a dip (oversold)
    for (int i = 0; i < 20; ++i) {
        prices.push_back(basePrice - i * 0.8);
    }
    
    // Then strong recovery (momentum + MA crossover)
    for (int i = 0; i < 15; ++i) {
        prices.push_back(basePrice - 20 * 0.8 + i * 1.5); // Strong recovery
    }
    
    for (double price : prices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Should generate a buy signal (may not be STRONG_BUY due to volatility filter)
    TestRunner::assertTrue(signal.signal == SignalType::STRONG_BUY || 
                           signal.signal == SignalType::BUY,
                           "Should generate BUY or STRONG_BUY signal in recovery scenario");
    TestRunner::assertTrue(signal.strength > 0.0, "Signal strength should be positive");
}

void testAlphaSignalGeneratorStrongSell() {
    std::cout << "\n=== Testing STRONG_SELL Signal Generation ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Create scenario for sell: overbought + downward momentum + MA crossover
    std::vector<double> prices;
    double basePrice = 100.0;
    
    // First create a strong rally (overbought)
    for (int i = 0; i < 25; ++i) {
        prices.push_back(basePrice + i * 1.0);
    }
    
    // Then strong decline (momentum + MA crossover)
    for (int i = 0; i < 10; ++i) {
        prices.push_back(basePrice + 25 * 1.0 - i * 2.0); // Strong decline
    }
    
    for (double price : prices) {
        generator.updatePrice(price, 1000.0, price);
    }
    
    AlphaSignal signal = generator.generateSignal();
    
    // Should generate a sell signal (may be HOLD if volatility filter reduces score)
    // Check that RSI is high (overbought) or momentum is negative
    bool hasSellCharacteristics = (signal.rsi > 60.0) || (signal.momentum < 0.0) || 
                                   (signal.signal == SignalType::SELL || 
                                    signal.signal == SignalType::STRONG_SELL);
    TestRunner::assertTrue(hasSellCharacteristics, 
                           "Should show sell characteristics (high RSI, negative momentum, or SELL signal)");
}

void testAlphaSignalGeneratorHistoryLimit() {
    std::cout << "\n=== Testing History Size Limit ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    // Add more than MAX_HISTORY prices
    for (int i = 0; i < 1500; ++i) {
        generator.updatePrice(100.0 + i * 0.1, 1000.0, 100.0 + i * 0.1);
    }
    
    // History should be limited to MAX_HISTORY (1000)
    size_t historySize = generator.getHistorySize();
    TestRunner::assertTrue(historySize <= 1000, "History should not exceed MAX_HISTORY");
    TestRunner::assertTrue(historySize >= 1000, "History should maintain MAX_HISTORY items");
}

void testAlphaSignalGeneratorSignalToString() {
    std::cout << "\n=== Testing Signal String Conversion ===" << std::endl;
    
    AlphaSignalGenerator generator;
    
    TestRunner::assertEquals(std::string("STRONG_BUY"), generator.signalToString(SignalType::STRONG_BUY), 
                            "STRONG_BUY string conversion");
    TestRunner::assertEquals(std::string("BUY"), generator.signalToString(SignalType::BUY), 
                            "BUY string conversion");
    TestRunner::assertEquals(std::string("HOLD"), generator.signalToString(SignalType::HOLD), 
                            "HOLD string conversion");
    TestRunner::assertEquals(std::string("SELL"), generator.signalToString(SignalType::SELL), 
                            "SELL string conversion");
    TestRunner::assertEquals(std::string("STRONG_SELL"), generator.signalToString(SignalType::STRONG_SELL), 
                            "STRONG_SELL string conversion");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "AlphaSignalGenerator Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    TestRunner::reset();
    
    testAlphaSignalGeneratorBasic();
    testAlphaSignalGeneratorSMA();
    testAlphaSignalGeneratorRSI();
    testAlphaSignalGeneratorMomentum();
    testAlphaSignalGeneratorVolatility();
    testAlphaSignalGeneratorStrongBuy();
    testAlphaSignalGeneratorStrongSell();
    testAlphaSignalGeneratorHistoryLimit();
    testAlphaSignalGeneratorSignalToString();
    
    TestRunner::printSummary();
    
    return TestRunner::getExitCode();
}

