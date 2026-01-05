#ifndef ALPHASIGNALGENERATOR_H
#define ALPHASIGNALGENERATOR_H

#include <deque>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <mutex>
#include <string>

enum class SignalType {
    STRONG_BUY = 2,
    BUY = 1,
    HOLD = 0,
    SELL = -1,
    STRONG_SELL = -2
};

struct AlphaSignal {
    SignalType signal;
    double strength;  // 0.0 to 1.0
    std::string reason;
    double price;
    double sma_short;
    double sma_long;
    double rsi;
    double momentum;
    double volatility;
};

class AlphaSignalGenerator {
private:
    // Price history for indicators
    std::deque<double> priceHistory;
    std::deque<double> volumeHistory;
    std::deque<double> vwapHistory;
    
    // Indicator parameters
    static constexpr size_t SHORT_MA_PERIOD = 10;
    static constexpr size_t LONG_MA_PERIOD = 30;
    static constexpr size_t RSI_PERIOD = 14;
    static constexpr size_t MOMENTUM_PERIOD = 10;
    static constexpr size_t VOLATILITY_PERIOD = 20;
    static constexpr size_t MAX_HISTORY = 1000;
    
    mutable std::mutex mtx;
    
    // Helper functions for indicators
    double calculateSMA(const std::deque<double>& data, size_t period) const {
        if (data.size() < period) return 0.0;
        double sum = 0.0;
        for (size_t i = data.size() - period; i < data.size(); ++i) {
            sum += data[i];
        }
        return sum / period;
    }
    
    double calculateEMA(const std::deque<double>& data, size_t period) const {
        if (data.size() < period) return 0.0;
        double multiplier = 2.0 / (period + 1.0);
        double ema = data[data.size() - period];
        for (size_t i = data.size() - period + 1; i < data.size(); ++i) {
            ema = (data[i] - ema) * multiplier + ema;
        }
        return ema;
    }
    
    double calculateRSI(const std::deque<double>& prices, size_t period) const {
        if (prices.size() < period + 1) return 50.0; // Neutral RSI
        
        std::vector<double> gains, losses;
        for (size_t i = prices.size() - period; i < prices.size(); ++i) {
            double change = prices[i] - prices[i - 1];
            if (change > 0) {
                gains.push_back(change);
                losses.push_back(0.0);
            } else {
                gains.push_back(0.0);
                losses.push_back(-change);
            }
        }
        
        double avgGain = std::accumulate(gains.begin(), gains.end(), 0.0) / period;
        double avgLoss = std::accumulate(losses.begin(), losses.end(), 0.0) / period;
        
        if (avgLoss == 0.0) return 100.0;
        double rs = avgGain / avgLoss;
        return 100.0 - (100.0 / (1.0 + rs));
    }
    
    double calculateMomentum(const std::deque<double>& prices, size_t period) const {
        if (prices.size() < period + 1) return 0.0;
        double current = prices.back();
        double past = prices[prices.size() - period - 1];
        return ((current - past) / past) * 100.0; // Percentage change
    }
    
    double calculateVolatility(const std::deque<double>& prices, size_t period) const {
        if (prices.size() < period + 1) return 0.0;
        
        double mean = calculateSMA(prices, period);
        double variance = 0.0;
        for (size_t i = prices.size() - period; i < prices.size(); ++i) {
            double diff = prices[i] - mean;
            variance += diff * diff;
        }
        variance /= period;
        return std::sqrt(variance) / mean * 100.0; // Coefficient of variation as percentage
    }
    
    SignalType determineSignal(double sma_short, double sma_long, double rsi, 
                              double momentum, double volatility, double currentPrice) const {
        int score = 0;
        
        // Moving average crossover
        if (sma_short > sma_long) score += 1;
        else if (sma_short < sma_long) score -= 1;
        
        // RSI signals
        if (rsi < 30) score += 2; // Oversold - strong buy
        else if (rsi < 40) score += 1; // Buy
        else if (rsi > 70) score -= 2; // Overbought - strong sell
        else if (rsi > 60) score -= 1; // Sell
        
        // Momentum signals
        if (momentum > 2.0) score += 1; // Strong upward momentum
        else if (momentum < -2.0) score -= 1; // Strong downward momentum
        
        // Volatility filter (lower volatility = more reliable signals)
        if (volatility > 5.0) {
            score = (score > 0) ? score - 1 : score + 1; // Reduce signal strength in high volatility
        }
        
        // Convert score to signal type
        if (score >= 3) return SignalType::STRONG_BUY;
        if (score >= 1) return SignalType::BUY;
        if (score <= -3) return SignalType::STRONG_SELL;
        if (score <= -1) return SignalType::SELL;
        return SignalType::HOLD;
    }
    
    double calculateSignalStrength(SignalType signal, double rsi, double momentum) const {
        double strength = 0.5; // Base strength
        
        // RSI contribution
        if (signal == SignalType::STRONG_BUY || signal == SignalType::STRONG_SELL) {
            strength += 0.3;
        } else if (signal == SignalType::BUY || signal == SignalType::SELL) {
            strength += 0.2;
        }
        
        // Momentum contribution
        double momentumStrength = std::min(std::abs(momentum) / 5.0, 0.2);
        strength += momentumStrength;
        
        return std::min(strength, 1.0);
    }
    
    std::string getSignalReason(SignalType signal, double sma_short, double sma_long, 
                                double rsi, double momentum) const {
        std::string reason = "";
        
        if (sma_short > sma_long) reason += "MA↑";
        else if (sma_short < sma_long) reason += "MA↓";
        
        if (rsi < 30) reason += " RSI_OS";
        else if (rsi > 70) reason += " RSI_OB";
        else if (rsi < 50) reason += " RSI↓";
        else reason += " RSI↑";
        
        if (momentum > 2.0) reason += " Mom↑";
        else if (momentum < -2.0) reason += " Mom↓";
        
        return reason;
    }

public:
    AlphaSignalGenerator() = default;
    
    void updatePrice(double price, double volume, double vwap) {
        std::lock_guard<std::mutex> lock(mtx);
        
        priceHistory.push_back(price);
        volumeHistory.push_back(volume);
        vwapHistory.push_back(vwap);
        
        // Limit history size
        if (priceHistory.size() > MAX_HISTORY) {
            priceHistory.pop_front();
            volumeHistory.pop_front();
            vwapHistory.pop_front();
        }
    }
    
    AlphaSignal generateSignal() const {
        std::lock_guard<std::mutex> lock(mtx);
        
        AlphaSignal signal;
        signal.signal = SignalType::HOLD;
        signal.strength = 0.0;
        signal.reason = "Insufficient data";
        
        // Need minimum data for indicators
        if (priceHistory.size() < LONG_MA_PERIOD + 1) {
            return signal;
        }
        
        // Calculate indicators
        double sma_short = calculateSMA(priceHistory, SHORT_MA_PERIOD);
        double sma_long = calculateSMA(priceHistory, LONG_MA_PERIOD);
        double rsi = calculateRSI(priceHistory, RSI_PERIOD);
        double momentum = calculateMomentum(priceHistory, MOMENTUM_PERIOD);
        double volatility = calculateVolatility(priceHistory, VOLATILITY_PERIOD);
        double currentPrice = priceHistory.back();
        
        // Generate signal
        signal.signal = determineSignal(sma_short, sma_long, rsi, momentum, volatility, currentPrice);
        signal.strength = calculateSignalStrength(signal.signal, rsi, momentum);
        signal.reason = getSignalReason(signal.signal, sma_short, sma_long, rsi, momentum);
        signal.price = currentPrice;
        signal.sma_short = sma_short;
        signal.sma_long = sma_long;
        signal.rsi = rsi;
        signal.momentum = momentum;
        signal.volatility = volatility;
        
        return signal;
    }
    
    std::string signalToString(SignalType signal) const {
        switch (signal) {
            case SignalType::STRONG_BUY: return "STRONG_BUY";
            case SignalType::BUY: return "BUY";
            case SignalType::HOLD: return "HOLD";
            case SignalType::SELL: return "SELL";
            case SignalType::STRONG_SELL: return "STRONG_SELL";
            default: return "UNKNOWN";
        }
    }
    
    size_t getHistorySize() const {
        std::lock_guard<std::mutex> lock(mtx);
        return priceHistory.size();
    }
};

#endif

