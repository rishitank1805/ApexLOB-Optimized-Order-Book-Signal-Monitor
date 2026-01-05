#include "OrderBook.h"
#include "AlphaSignalGenerator.h"
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXSocketTLSOptions.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <exception>
#include <iomanip>
#include <sstream>
#include <numeric>

using json = nlohmann::json;

// Helper function to get current time as string
std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// Global timing statistics
struct TimingStats {
    std::chrono::high_resolution_clock::time_point connection_start;
    std::chrono::high_resolution_clock::time_point first_message_time;
    bool first_message_received = false;
    int total_messages = 0;
    double total_processing_time_ms = 0.0;
    std::mutex mtx;
} timing_stats;

void displayMetricsWithSignals(OrderBook& ob, AlphaSignalGenerator& signalGen, 
                                double processingTimeMs, int totalMessages, 
                                double totalProcessingTimeMs) {
    // Get current metrics
    double lastPrice = ob.getLastTradePrice();
    double vwap = ob.getVWAP();
    uint32_t volume = ob.getTotalVolume();
    
    // Update signal generator with latest data
    if (lastPrice > 0.0) {
        signalGen.updatePrice(lastPrice, volume, vwap);
    }
    
    // Generate alpha signal
    AlphaSignal signal = signalGen.generateSignal();
    
    // Display metrics
    double avgProcessingTime = (totalMessages > 0) ? (totalProcessingTimeMs / totalMessages) : 0.0;
    
    std::cout << "\r[LOB] Last: " << std::fixed << std::setprecision(2) << lastPrice 
              << " | VWAP: " << vwap << " | Vol: " << volume;
    
    if (totalMessages > 0) {
        std::cout << " | Msg: " << totalMessages 
                  << " | AvgProc: " << std::setprecision(3) << avgProcessingTime << "ms";
    }
    
    // Display alpha signal
    if (signalGen.getHistorySize() >= 31) { // Need enough data for indicators
        std::cout << " | [ALPHA] " << signalGen.signalToString(signal.signal)
                  << " (" << std::setprecision(1) << (signal.strength * 100) << "%)"
                  << " | RSI: " << std::setprecision(1) << signal.rsi
                  << " | Mom: " << std::setprecision(2) << signal.momentum << "%"
                  << " | " << signal.reason;
    } else {
        std::cout << " | [ALPHA] Collecting data... (" << signalGen.getHistorySize() << "/31)";
    }
    
    std::cout << std::flush;
}

void startLiveFeed(OrderBook& ob, AlphaSignalGenerator& signalGen) {
    // Initialize network system (required for Windows)
    ix::initNetSystem();

    // Record connection start time
    timing_stats.connection_start = std::chrono::high_resolution_clock::now();

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
    webSocket.setOnMessageCallback([&ob, &signalGen](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            auto msg_start = std::chrono::high_resolution_clock::now();
            
            // Record first message time
            {
                std::lock_guard<std::mutex> lock(timing_stats.mtx);
                if (!timing_stats.first_message_received) {
                    timing_stats.first_message_received = true;
                    timing_stats.first_message_time = msg_start;
                    auto connection_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                        msg_start - timing_stats.connection_start).count();
                    std::cout << "\n[" << getCurrentTimeString() << "] [INFO] First message received in " 
                              << connection_time << "ms" << std::endl;
                }
            }
            
            try {
                auto j = json::parse(msg->str);
                
                // Validate required fields exist
                if (!j.contains("p") || !j.contains("q") || !j.contains("m") || !j.contains("a")) {
                    std::cerr << "[" << getCurrentTimeString() << "] [WARNING] Missing required fields in message" << std::endl;
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
                
                // Calculate processing time
                auto msg_end = std::chrono::high_resolution_clock::now();
                auto processing_time_ms = std::chrono::duration<double, std::milli>(
                    msg_end - msg_start).count();
                
                // Update timing statistics
                {
                    std::lock_guard<std::mutex> lock(timing_stats.mtx);
                    timing_stats.total_messages++;
                    timing_stats.total_processing_time_ms += processing_time_ms;
                }
                
                // Display metrics with alpha signals
                displayMetricsWithSignals(ob, signalGen, processing_time_ms, 
                                         timing_stats.total_messages, 
                                         timing_stats.total_processing_time_ms);
            } catch (const json::parse_error& e) {
                std::cerr << "[" << getCurrentTimeString() << "] [ERROR] JSON parse error: " << e.what() << std::endl;
            } catch (const json::type_error& e) {
                std::cerr << "[" << getCurrentTimeString() << "] [ERROR] JSON type error: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[" << getCurrentTimeString() << "] [ERROR] Error processing message: " << e.what() << std::endl;
            }
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            std::cerr << "[" << getCurrentTimeString() << "] [ERROR] WebSocket error: " << msg->errorInfo.reason << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Open) {
            auto open_time = std::chrono::high_resolution_clock::now();
            auto connection_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                open_time - timing_stats.connection_start).count();
            std::cout << "[" << getCurrentTimeString() << "] [INFO] Connected to Binance WebSocket" << std::endl;
            std::cout << "[INFO] Connection established in " << connection_time << "ms" << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - timing_stats.connection_start).count();
            std::cout << "\n[" << getCurrentTimeString() << "] [INFO] WebSocket connection closed: " 
                      << msg->closeInfo.reason << std::endl;
            std::cout << "[INFO] Connection duration: " << duration << " seconds" << std::endl;
            std::lock_guard<std::mutex> lock(timing_stats.mtx);
            double msgs_per_sec = (duration > 0) ? (timing_stats.total_messages / duration) : 0.0;
            std::cout << "[INFO] Total messages processed: " << timing_stats.total_messages << std::endl;
            std::cout << "[INFO] Messages per second: " << std::fixed << std::setprecision(2) << msgs_per_sec << std::endl;
            if (timing_stats.total_messages > 0) {
                std::cout << "[INFO] Average processing time: " << std::setprecision(3) 
                          << (timing_stats.total_processing_time_ms / timing_stats.total_messages) << " ms" << std::endl;
            }
        }
    });

    // Use blocking connect method for more reliable connection
    std::cout << "Connecting to Binance WebSocket..." << std::endl;
    auto connect_start = std::chrono::high_resolution_clock::now();
    auto result = webSocket.connect(60);  // 60 second timeout
    
    if (!result.success) {
        std::cerr << "[" << getCurrentTimeString() << "] [ERROR] Failed to connect: " << result.errorStr << std::endl;
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
    AlphaSignalGenerator signalGen;
    
    std::cout << "Connecting to Binance BTC/USDT Live Feed..." << std::endl;
    std::cout << "Alpha Signal Generation: ENABLED" << std::endl;
    std::cout << "Indicators: SMA(10/30), RSI(14), Momentum(10), Volatility(20)" << std::endl;
    std::cout << "Signal Types: STRONG_BUY, BUY, HOLD, SELL, STRONG_SELL" << std::endl;
    std::cout << std::endl;
    
    startLiveFeed(ob, signalGen);
    return 0;
}