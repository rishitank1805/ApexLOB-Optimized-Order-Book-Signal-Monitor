# ApexLOB - Optimized Order Book Signal Monitor

A high-performance C++ application that connects to Binance WebSocket API to stream live BTC/USDT trading data and maintain an optimized order book with real-time metrics.

## Features

- 🔴 **Live WebSocket Connection** - Real-time connection to Binance aggregated trade stream
- 📊 **Order Book Management** - Efficient order book implementation with bid/ask matching
- 📈 **Real-time Metrics** - Displays Last Trade Price, VWAP (Volume Weighted Average Price), and Total Volume
- 🔒 **Secure TLS Connection** - Full SSL/TLS support with OpenSSL
- ⚡ **High Performance** - Optimized C++ implementation with efficient data structures

## Prerequisites

- **macOS** (tested on macOS with Homebrew)
- **C++17 compatible compiler** (Clang/GCC)
- **CMake 3.10 or higher**
- **Homebrew** package manager (for macOS)

## Dependencies

The project requires the following dependencies:

- **IXWebSocket** - WebSocket library (bundled locally)
- **nlohmann/json** - JSON parsing library
- **OpenSSL** - TLS/SSL support
- **ZLIB** - Compression library
- **Threads** - Multi-threading support

### Installing Dependencies

#### macOS (using Homebrew)

```bash
# Install CMake (if not already installed)
brew install cmake

# Install OpenSSL
brew install openssl

# Install nlohmann/json (via Homebrew or it will be found automatically)
brew install nlohmann-json

# Install ZLIB (usually pre-installed on macOS)
brew install zlib
```

**Note:** IXWebSocket is included in the `lib/` directory, so no additional installation is needed.

## Building the Project

### Step 1: Clone or Navigate to the Project Directory

```bash
cd "path/to/ApexLOB-Optimized-Order-Book-Signal-Monitor"
```

### Step 2: Create Build Directory

```bash
mkdir -p build
cd build
```

### Step 3: Configure with CMake

```bash
cmake ..
```

This will:
- Detect all required dependencies
- Configure the build system
- Show status messages for found libraries

You should see output like:
```
-- Using local IXWebSocket library
-- IXWebSocket Include: /path/to/lib/ixwebsocket/include
-- IXWebSocket Library: /path/to/lib/ixwebsocket/lib/libixwebsocket.a
-- Found Security framework for TLS support
-- Found CoreFoundation framework
-- Configuring done
-- Generating done
```

### Step 4: Build the Executable

```bash
make
```

Or for faster builds with multiple cores:

```bash
make -j$(sysctl -n hw.ncpu)
```

The executable `TradingEngine` will be created in the `build/` directory.

## Running the Program

### Run from Build Directory

```bash
cd build
./TradingEngine
```

### Run from Project Root

```bash
./build/TradingEngine
```

### Expected Output

When running, you should see:

```
Connecting to Binance BTC/USDT Live Feed...
Connecting to Binance WebSocket...
Connected successfully! Waiting for data...
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234
```

The metrics will update in real-time as trades are received from Binance.

### Stopping the Program

Press `Ctrl+C` to stop the program gracefully.

## Project Structure

```
ApexLOB-Optimized-Order-Book-Signal-Monitor/
├── CMakeLists.txt          # CMake build configuration
├── main.cpp                # Main application entry point
├── Order.h                 # Order and LimitLevel data structures
├── OrderBook.h             # Order book implementation
├── README.md               # This file
├── build/                  # Build directory (generated)
│   └── TradingEngine       # Compiled executable
└── lib/                    # Local libraries
    └── ixwebsocket/        # IXWebSocket library (headers + static lib)
        ├── include/        # Header files
        └── lib/            # Static library file
```

## Configuration

### Changing the Trading Pair

To monitor a different trading pair, edit `main.cpp` and change the WebSocket URL:

```cpp
std::string url = "wss://stream.binance.com:443/ws/btcusdt@aggTrade";
```

For example, for ETH/USDT:
```cpp
std::string url = "wss://stream.binance.com:443/ws/ethusdt@aggTrade";
```

### WebSocket Endpoint

The program connects to Binance's aggregated trade stream on port 443 (standard HTTPS port). The endpoint format is:
```
wss://stream.binance.com:443/ws/{symbol}@aggTrade
```

## Troubleshooting

### Build Issues

**Problem:** CMake cannot find dependencies
```bash
# Solution: Ensure Homebrew packages are installed and paths are correct
brew doctor
brew update
```

**Problem:** IXWebSocket not found
```bash
# Solution: Verify lib/ixwebsocket directory exists with headers and library
ls -la lib/ixwebsocket/include/ixwebsocket/IXWebSocket.h
ls -la lib/ixwebsocket/lib/libixwebsocket.a
```

### Connection Issues

**Problem:** "TLS support is not enabled"
- Solution: Ensure OpenSSL is installed: `brew install openssl`
- The library should be built with OpenSSL support

**Problem:** "Cancellation requested" or connection timeout
- Solution: Check internet connection
- Verify Binance API is accessible: `curl -I https://stream.binance.com:443`
- Ensure firewall is not blocking port 443

**Problem:** "Unable to connect"
- Solution: Verify Binance WebSocket endpoint is accessible
- Check if your network blocks WebSocket connections
- Try using port 9443 as an alternative (edit URL in main.cpp)

### Runtime Issues

**Problem:** No data received
- Solution: Wait a few seconds for trades to arrive
- Check that Binance API is operational
- Verify the trading pair symbol is correct (lowercase, e.g., "btcusdt")

**Problem:** Program exits immediately
- Solution: Check error messages for connection failures
- Ensure all dependencies are correctly linked

## Technical Details

### Order Book Implementation

- Uses `std::map` with custom comparators for efficient bid/ask storage
- Thread-safe operations with `std::mutex`
- Automatic order matching on submission
- Efficient volume aggregation at each price level

### WebSocket Connection

- Blocking connection mode for reliable initial handshake
- Automatic ping/pong handling (Binance sends pings every 20 seconds)
- 60-second connection timeout
- TLS 1.2+ with system certificate validation

### Performance

- Optimized data structures for O(log n) operations
- Minimal memory allocations
- Real-time metric calculations
- Efficient JSON parsing with nlohmann/json

## License

[Add your license here]

## Contributing

[Add contribution guidelines if applicable]

## Author

[Add author information]

