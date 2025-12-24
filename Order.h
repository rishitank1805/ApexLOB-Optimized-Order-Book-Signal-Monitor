#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <list>
#include <memory>
#include <chrono>

enum class Side { Buy, Sell };

struct Order {
    uint64_t id;
    double price;
    uint32_t quantity;
    Side side;
    std::chrono::high_resolution_clock::time_point entryTime;

    Order(uint64_t id, double price, uint32_t qty, Side side)
        : id(id), price(price), quantity(qty), side(side) {
        entryTime = std::chrono::high_resolution_clock::now();
    }
};

struct LimitLevel {
    double price;
    uint32_t totalVolume = 0;
    std::list<std::shared_ptr<Order>> orders; 
};

#endif