#include "OrderBook.h"
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXSocketTLSOptions.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <exception>

using json = nlohmann::json;

void startLiveFeed(OrderBook& ob) {
    // Initialize network system (required for Windows)
    ix::initNetSystem();

    ix::WebSocket webSocket;
    // Use port 443 instead of 9443 - more reliable and less likely to be blocked
    std::string url = "wss://stream.binance.com:443/ws/btcusdt@aggTrade";
    webSocket.setUrl(url);
    
    // Configure TLS options - use system default CA certificates
    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "SYSTEM";  // Use system certificate store
    tlsOptions.ciphers = "DEFAULT";
    // Temporarily disable hostname validation to troubleshoot connection issues
    tlsOptions.disable_hostname_validation = false;
    webSocket.setTLSOptions(tlsOptions);
    
    // Set connection timeout - Binance requires proper timeouts
    webSocket.setHandshakeTimeout(60);
    
    // Enable pong responses - Binance sends ping every 20 seconds
    webSocket.enablePong();
    
    // Set ping interval to keep connection alive (optional, Binance will ping us)
    webSocket.setPingInterval(30);

    // Add error handling
    webSocket.setOnMessageCallback([&ob](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            try {
                auto j = json::parse(msg->str);
                
                // Validate required fields exist
                if (!j.contains("p") || !j.contains("q") || !j.contains("m") || !j.contains("a")) {
                    std::cerr << "Warning: Missing required fields in message" << std::endl;
                    return;
                }
                
                // Binance 'aggTrade' fields: p=price, q=quantity, m=isBuyerMaker
                double price = std::stod(j["p"].get<std::string>());
                double quantity = std::stod(j["q"].get<std::string>());
                bool isSell = j["m"].get<bool>(); 
                
                auto order = std::make_shared<Order>(
                    j["a"].get<uint64_t>(), // Aggregate trade ID
                    price, 
                    static_cast<uint32_t>(quantity * 1000), // Scale for integer qty
                    isSell ? Side::Sell : Side::Buy
                );

                ob.submitOrder(order);
                ob.displayMetrics();
            } catch (const json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            } catch (const json::type_error& e) {
                std::cerr << "JSON type error: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            std::cerr << "WebSocket error: " << msg->errorInfo.reason << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "Connected to Binance WebSocket" << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "WebSocket connection closed: " << msg->closeInfo.reason << std::endl;
        }
    });

    // Use blocking connect method for more reliable connection
    std::cout << "Connecting to Binance WebSocket..." << std::endl;
    auto result = webSocket.connect(60);  // 60 second timeout
    
    if (!result.success) {
        std::cerr << "Failed to connect: " << result.errorStr << std::endl;
        if (result.http_status != 0) {
            std::cerr << "HTTP Status Code: " << result.http_status << std::endl;
        }
        ix::uninitNetSystem();
        return;
    }
    
    std::cout << "Connected successfully! Waiting for data..." << std::endl;
    
    // Run the WebSocket in the current thread
    webSocket.run();
    
    webSocket.stop();
    ix::uninitNetSystem();
}

int main() {
    OrderBook ob;
    std::cout << "Connecting to Binance BTC/USDT Live Feed..." << std::endl;
    startLiveFeed(ob);
    return 0;
}