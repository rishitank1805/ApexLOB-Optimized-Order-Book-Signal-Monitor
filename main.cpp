#include "OrderBook.h"
#include <thread>
#include <vector>
#include <random>

void marketSimulator(OrderBook& ob) {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> priceDist(95.0, 105.0);
    std::uniform_int_distribution<int> sideDist(0, 1);
    uint64_t id = 1;

    std::cout << "Starting Real-Time Market Feed..." << std::endl;

    while (true) {
        double price = std::round(priceDist(generator));
        Side side = (sideDist(generator) == 0) ? Side::Buy : Side::Sell;
        
        auto order = std::make_shared<Order>(id++, price, 10, side);
        ob.submitOrder(order);
        ob.displayMetrics();

        // Simulate frequency of 2 orders per second
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    OrderBook ob;

    // Run the simulator in a background thread
    std::thread feed(marketSimulator, std::ref(ob));

    // Keep the main thread alive
    feed.join();

    return 0;
}