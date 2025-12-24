#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <map>
#include <mutex>
#include <iomanip>

class OrderBook {
private:
    std::map<double, LimitLevel, std::greater<double>> bids;
    std::map<double, LimitLevel, std::less<double>> asks;
    mutable std::mutex mtx;

    double lastTradePrice = 0.0;
    uint32_t totalVolumeTraded = 0;
    double cumulativeNotional = 0.0;

public:
    void submitOrder(std::shared_ptr<Order> order) {
        std::lock_guard<std::mutex> lock(mtx);
        if (order->side == Side::Buy) {
            matchOrder(order, asks);
            if (order->quantity > 0) addLimit(order, bids);
        } else {
            matchOrder(order, bids);
            if (order->quantity > 0) addLimit(order, asks);
        }
    }

private:
    template <typename T>
    void matchOrder(std::shared_ptr<Order> order, T& oppositeSide) {
        auto it = oppositeSide.begin();
        while (it != oppositeSide.end() && order->quantity > 0) {
            bool canMatch = (order->side == Side::Buy) ? (order->price >= it->first) : (order->price <= it->first);
            if (!canMatch) break;

            LimitLevel& level = it->second;
            auto orderIt = level.orders.begin();
            while (orderIt != level.orders.end() && order->quantity > 0) {
                uint32_t tradedQty = std::min(order->quantity, (*orderIt)->quantity);
                lastTradePrice = it->first;
                totalVolumeTraded += tradedQty;
                cumulativeNotional += (tradedQty * lastTradePrice);
                order->quantity -= tradedQty;
                (*orderIt)->quantity -= tradedQty;
                level.totalVolume -= tradedQty;
                if ((*orderIt)->quantity == 0) orderIt = level.orders.erase(orderIt);
                else ++orderIt;
            }
            if (level.orders.empty()) it = oppositeSide.erase(it);
            else ++it;
        }
    }

    template <typename T>
    void addLimit(std::shared_ptr<Order> order, T& sideMap) {
        auto& level = sideMap[order->price];
        level.price = order->price;
        level.totalVolume += order->quantity;
        level.orders.push_back(order);
    }

public:
    void displayMetrics() const {
        std::lock_guard<std::mutex> lock(mtx);
        double vwap = (totalVolumeTraded > 0) ? (cumulativeNotional / totalVolumeTraded) : 0;
        std::cout << "\r[LOB] Last: " << std::fixed << std::setprecision(2) << lastTradePrice 
                  << " | VWAP: " << vwap << " | Vol: " << totalVolumeTraded << std::flush;
    }
};

#endif