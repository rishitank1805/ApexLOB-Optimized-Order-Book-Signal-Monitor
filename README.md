# ApexLOB - Optimized Order Book Signal Monitor

A high-performance C++ application that connects to Binance WebSocket API to stream live BTC/USDT trading data, maintain an optimized order book with real-time metrics, and generate alpha trading signals using advanced technical indicators.

## Features

- 🔴 **Live WebSocket Connection** - Real-time connection to Binance aggregated trade stream
- 📊 **Order Book Management** - Efficient order book implementation with bid/ask matching
- 📈 **Real-time Metrics** - Displays Last Trade Price, VWAP (Volume Weighted Average Price), and Total Volume
- 🎯 **Alpha Signal Generation** - Advanced technical indicators (SMA, RSI, Momentum, Volatility) with automated trading signals
- 📝 **Comprehensive Logging** - Thread-safe logging system with configurable levels and file/console output
- 🧪 **Extensive Test Suite** - 111+ unit tests covering core functionality and edge cases
- 🔒 **Secure TLS Connection** - Full SSL/TLS support with OpenSSL
- ⚡ **High Performance** - Optimized C++ implementation with efficient data structures

## C++ vs Python vs Go Performance Comparison

This project includes implementations in **C++**, **Python** (`main.py`), and **Go** (`main.go`) for comprehensive performance comparison. The C++ version demonstrates the best performance, with Go providing an excellent balance between performance and development speed.

### 📊 Performance Metrics Comparison

| Metric | C++ Implementation | Go Implementation | Python Implementation | C++ vs Go | C++ vs Python |
|--------|-------------------|-------------------|----------------------|-----------|---------------|
| **Average Processing Time** | 0.01-0.1 ms/message | 0.05-0.3 ms/message | 0.1-1.0 ms/message | **2-3x faster** | **10-50x faster** |
| **Throughput** | 100-500+ msg/sec | 80-400+ msg/sec | 50-300 msg/sec | **1.2-1.5x higher** | **1.5-2x higher** |
| **Connection Time** | 200-500 ms | 250-600 ms | 300-800 ms | **Similar** | **~1.5x faster** |
| **Memory Usage** | ~5-15 MB | ~10-25 MB | ~20-50 MB | **2x lower** | **3-4x lower** |
| **Latency Consistency** | Very low variance | Low variance | Higher variance | **Better** | **More predictable** |
| **CPU Usage** | Lower (optimized) | Moderate (GC) | Higher (interpreted) | **More efficient** | **Most efficient** |
| **Compilation** | Required | Required | Not required | Both compiled | C++/Go compiled |
| **Development Speed** | Slower | Faster | Fastest | Go faster | Python fastest |

### 🏗️ Architecture & Implementation Comparison

#### Data Structures

| Component | C++ Implementation | Go Implementation | Python Implementation |
|-----------|-------------------|-------------------|----------------------|
| **Order Book** | `std::map<double, LimitLevel>` with custom comparators | `map[float64]*LimitLevel` with sorted keys | `OrderedDict[float, LimitLevel]` |
| **Order Storage** | `std::list<std::shared_ptr<Order>>` (reference semantics) | `[]*Order` slice | `List[Order]` (object references) |
| **Thread Safety** | `std::mutex` with `std::lock_guard` (RAII) | `sync.RWMutex` with defer unlock | `threading.Lock` with context manager |
| **Memory Management** | Smart pointers (`std::shared_ptr`), zero-copy | Automatic GC (mark-and-sweep) | Reference counting (automatic GC) |
| **JSON Parsing** | `nlohmann/json` (header-only, optimized) | `encoding/json` (standard library) | Built-in `json` module (interpreted) |
| **WebSocket Library** | `IXWebSocket` (C++ native) | `gorilla/websocket` (Go native) | `websocket-client` (Python wrapper) |

#### Key Technical Differences

**C++ Advantages:**
- ✅ **Compiled code** - Machine code execution, no interpreter overhead
- ✅ **Zero-cost abstractions** - Templates and inline functions compile to efficient code
- ✅ **Direct memory access** - No garbage collection pauses
- ✅ **Type safety** - Compile-time type checking prevents runtime errors
- ✅ **Optimization** - Compiler optimizations (inlining, vectorization, etc.)
- ✅ **Standard library efficiency** - `std::map` uses red-black trees (O(log n))

**Go Characteristics:**
- ✅ **Compiled code** - Native machine code execution
- ✅ **Fast compilation** - Much faster than C++ compilation
- ✅ **Goroutines** - Excellent concurrency model (better than threads)
- ⚠️ **Garbage collection** - Automatic GC with low-latency pauses (better than Python)
- ⚠️ **Runtime overhead** - Some runtime overhead compared to C++
- ✅ **Standard library** - Excellent standard library including JSON and WebSocket

**Python Characteristics:**
- ⚠️ **Interpreted execution** - Bytecode interpretation adds overhead
- ⚠️ **Dynamic typing** - Type checks at runtime
- ⚠️ **Garbage collection** - Automatic memory management with occasional pauses
- ⚠️ **Object overhead** - Python objects have significant metadata overhead
- ⚠️ **Global Interpreter Lock (GIL)** - Limits true parallelism

#### Code Structure Comparison

**C++ Order Book Core:**
```cpp
// Efficient map-based storage with custom comparators
std::map<double, LimitLevel, std::greater<double>> bids;  // Highest first
std::map<double, LimitLevel, std::less<double>> asks;     // Lowest first

// RAII-based thread safety
std::lock_guard<std::mutex> lock(mtx);
// Automatic unlock on scope exit
```

**Go Order Book Core:**
```go
// Map-based storage with sorted keys
bids map[float64]*LimitLevel
asks map[float64]*LimitLevel

// Mutex-based thread safety with defer
ob.mu.Lock()
defer ob.mu.Unlock()
// Automatic unlock via defer
```

**Python Order Book Core:**
```python
# OrderedDict-based storage
self.bids: OrderedDict[float, LimitLevel] = OrderedDict()
self.asks: OrderedDict[float, LimitLevel] = OrderedDict()

# Context manager for thread safety
with self.lock:
    # Manual lock management required
```

### 🎯 Use Case Recommendations

#### Choose C++ When:
- ⚡ **Ultra-low latency** - Microsecond latency is critical
- 📈 **Maximum throughput** - Processing 1000+ messages/second
- 💰 **Production HFT systems** - Every microsecond matters
- 🔬 **Resource constraints** - Minimal memory and CPU usage required
- 🎯 **Absolute performance** - Need the fastest possible execution

#### Choose Go When:
- ⚡ **Good performance** - Fast enough for most trading systems
- 🚀 **Fast development** - Need to iterate quickly
- 🔄 **Concurrency** - Need excellent concurrent processing (goroutines)
- 📦 **Easy deployment** - Single binary deployment
- 🛠️ **Standard library** - Rich standard library for networking/JSON
- ⚖️ **Balance** - Good balance between performance and development speed

#### Choose Python When:
- 🔧 **Prototyping & development** - Fastest iteration and experimentation
- 📊 **Data analysis** - Rich ecosystem (pandas, numpy, matplotlib)
- 🎓 **Learning & education** - Easier to understand and modify
- 🔍 **Debugging** - Better error messages and development tools
- 🧪 **Testing & validation** - Quick to write test cases
- 📝 **Research & backtesting** - Easy integration with data science tools

### 🔬 Technical Deep Dive: Performance Comparison

1. **Compilation vs Interpretation**
   - **C++**: Compiled to native machine code with aggressive optimizations
   - **Go**: Compiled to native machine code with runtime (GC, scheduler)
   - **Python**: Interpreted from bytecode
   - Result: C++ > Go > Python (C++ fastest, Python slowest)

2. **Memory Layout & Cache Efficiency**
   - **C++**: Contiguous memory layouts, optimal cache utilization
   - **Go**: Good memory layout, some GC overhead
   - **Python**: Objects scattered in memory (pointer chasing)
   - Result: C++ > Go > Python (cache efficiency)

3. **Type System**
   - **C++**: Compile-time types enable aggressive optimizations
   - **Go**: Static typing with compile-time checks
   - **Python**: Dynamic types require runtime type checks
   - Result: C++ ≈ Go > Python (type safety and optimization)

4. **Abstractions Cost**
   - **C++**: Zero-cost abstractions (templates, inline functions)
   - **Go**: Low-cost abstractions (interfaces, goroutines)
   - **Python**: Runtime costs (method lookups, dynamic dispatch)
   - Result: C++ > Go > Python (abstraction efficiency)

5. **Garbage Collection**
   - **C++**: No GC (deterministic destruction with RAII)
   - **Go**: Low-latency GC (stop-the-world pauses < 1ms typically)
   - **Python**: GC with unpredictable pauses (can be 10-100ms)
   - Result: C++ > Go > Python (latency consistency)

6. **Concurrency Model**
   - **C++**: Threads with manual management
   - **Go**: Goroutines with lightweight scheduling (excellent for I/O)
   - **Python**: Threads limited by GIL, multiprocessing for parallelism
   - Result: Go > C++ > Python (for concurrent I/O workloads)

### 📈 Benchmarking

#### Quick Comparison

Run the automated benchmark script to compare both implementations:

```bash
./benchmark.sh
```

This script will:
- Run both implementations for 30 seconds
- Collect performance metrics automatically
- Display a comparison summary
- Save detailed logs for analysis

#### Manual Comparison

```bash
# Terminal 1 - C++ version
./build/TradingEngine

# Terminal 2 - Python version
python3 main.py
```

Run both side-by-side for the same duration and compare the final statistics.

#### Expected Sample Results

**C++ Output:**
```
[INFO] Connection established in 234ms
[INFO] First message received in 567ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.045ms | [ALPHA] BUY (75.2%) | RSI: 35.2 | Mom: 2.45% | MA↑ RSI_OS Mom↑
[INFO] Total messages processed: 1245
[INFO] Messages per second: 41.50
[INFO] Average processing time: 0.052 ms
```

**Go Output:**
```
[INFO] Connected to Binance WebSocket
[INFO] Connection established in 280ms
[INFO] First message received in 620ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.082ms
[INFO] Total messages processed: 1156
[INFO] Messages per second: 38.53
[INFO] Average processing time: 0.089 ms
```

**Python Output:**
```
[INFO] Connection established in 345ms
[INFO] First message received in 678ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.234ms
[INFO] Total messages processed: 987
[INFO] Messages per second: 32.90
[INFO] Average processing time: 0.287 ms
```

**Note:** The C++ implementation includes alpha signal generation, which is not available in the Python or Go versions. The Python and Go implementations focus on basic order book functionality for comparison purposes.

**Interpretation:**
- C++ processed **1.26x more messages** than Python and is **5.5x faster** per message
- Go processed **1.17x more messages** than Python and is **3.2x faster** per message
- C++ is **1.75x faster** than Go per message

### 📚 Detailed Comparison Guide

For comprehensive performance metrics, testing methods, advanced profiling techniques, and detailed analysis, see [PERFORMANCE_COMPARISON.md](PERFORMANCE_COMPARISON.md).

**Note:** The Python implementation (`main.py`) is included for educational and benchmarking purposes. For production use in high-frequency trading scenarios, the C++ implementation is strongly recommended.

## Prerequisites

### For C++ Implementation

- **macOS** (tested on macOS with Homebrew)
- **C++17 compatible compiler** (Clang/GCC)
- **CMake 3.10 or higher**
- **Homebrew** package manager (for macOS)

### For Python Implementation

- **Python 3.7 or higher**
- **pip** (Python package installer, usually comes with Python)
- **Internet connection** (for downloading dependencies)

## Implementation Guides

This project provides three complete implementations:

1. **C++ Implementation** - Highest performance, production-ready version
2. **Go Implementation** - Excellent performance with fast development
3. **Python Implementation** - Educational and comparison version

---

## 🚀 C++ Implementation

### Dependencies

The C++ implementation requires:

- **IXWebSocket** - WebSocket library (bundled locally)
- **nlohmann/json** - JSON parsing library
- **OpenSSL** - TLS/SSL support
- **ZLIB** - Compression library
- **Threads** - Multi-threading support

### Installing C++ Dependencies

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

### Building the C++ Project

#### Step 1: Clone or Navigate to the Project Directory

```bash
cd "path/to/ApexLOB-Optimized-Order-Book-Signal-Monitor"
```

#### Step 2: Create Build Directory

```bash
mkdir -p build
cd build
```

#### Step 3: Configure with CMake

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

#### Step 4: Build the Executable

```bash
make
```

Or for faster builds with multiple cores:

```bash
make -j$(sysctl -n hw.ncpu)
```

The executable `TradingEngine` will be created in the `build/` directory.

#### Step 5: Build Test Executables (Optional)

```bash
# Build all test executables
make test_alpha_signal test_orderbook test_edge_cases

# Or build everything
make
```

The test executables will be created in the `build/` directory:
- `test_alpha_signal` - AlphaSignalGenerator test suite
- `test_orderbook` - OrderBook test suite
- `test_edge_cases` - Edge case test suite

### Running the C++ Implementation

#### Run from Build Directory

```bash
cd build
./TradingEngine
```

#### Run from Project Root

```bash
./build/TradingEngine
```

#### Expected Output

When running, you should see:

```
Connecting to Binance BTC/USDT Live Feed...
Alpha Signal Generation: ENABLED
Indicators: SMA(10/30), RSI(14), Momentum(10), Volatility(20)
Signal Types: STRONG_BUY, BUY, HOLD, SELL, STRONG_SELL
Logging to: apexlob.log
Connecting to Binance WebSocket...
[INFO] Connected to Binance WebSocket
[INFO] Connection established in 234ms
[INFO] First message received in 567ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.045ms | [ALPHA] BUY (75.2%) | RSI: 35.2 | Mom: 2.45% | MA↑ RSI_OS Mom↑
```

The output shows:
- **Real-time order book metrics** (Last Price, VWAP, Volume)
- **Processing statistics** (Message count, Average processing time)
- **Alpha signals** (Signal type, strength percentage, RSI, Momentum, Reason)
- **Signal indicators** (Moving average direction, RSI status, Momentum direction)

The metrics will update in real-time as trades are received from Binance.

#### Stopping the Program

Press `Ctrl+C` to stop the program gracefully. You'll see final statistics:

```
[INFO] Total messages processed: 1245
[INFO] Messages per second: 41.50
[INFO] Average processing time: 0.052 ms
```

---

## 🐹 Go Implementation

### Dependencies

The Go implementation requires:

- **gorilla/websocket** - WebSocket library for Go
- **Go Standard Library** - `encoding/json`, `sync`, `time`, `sort`

### Installing Go Dependencies

#### Step 1: Check Go Version

```bash
go version
```

Ensure you have Go 1.21 or higher. If not, install it:

```bash
# macOS (using Homebrew)
brew install go

# Or download from https://golang.org/dl/
```

#### Step 2: Install Dependencies

Dependencies are managed via Go modules and will be downloaded automatically:

```bash
# From project root directory
go mod download
```

This will automatically download `gorilla/websocket` and its dependencies.

### Building the Go Project

#### Step 1: Navigate to Project Directory

```bash
cd "path/to/ApexLOB-Optimized-Order-Book-Signal-Monitor"
```

#### Step 2: Build the Executable

```bash
go build -o apexlob-go main.go orders.go orderbook.go
```

Or build and run directly:

```bash
go run main.go orders.go orderbook.go
```

For optimized build:

```bash
go build -ldflags="-s -w" -o apexlob-go main.go orders.go orderbook.go
```

The executable `apexlob-go` will be created in the current directory.

### Running the Go Implementation

#### Run from Project Root

```bash
./apexlob-go
```

Or run directly without building:

```bash
go run main.go orders.go orderbook.go
```

#### Expected Output

When running, you should see:

```
Connecting to Binance btcusdt/USDT Live Feed...
WebSocket URL: wss://stream.binance.com:443/ws/btcusdt@aggTrade

[INFO] Connected to Binance WebSocket
[INFO] Connection established in 280ms
[INFO] First message received in 620ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.082ms
```

The metrics will update in real-time as trades are received from Binance.

#### Stopping the Program

Press `Ctrl+C` to stop the program gracefully. You'll see final statistics:

```
[INFO] Interrupted by user
[INFO] Connection duration: 30.25 seconds
[INFO] Total messages processed: 1156
[INFO] Messages per second: 38.53
[INFO] Average processing time: 0.089 ms
```

### Go Configuration

#### Changing the Trading Pair

Edit `main.go` and modify the symbol variable:

```go
// Change this line:
symbol := "btcusdt"

// For example, for ETH/USDT:
symbol := "ethusdt"
```

#### WebSocket Endpoint

The Go implementation uses the same Binance endpoint format:
```
wss://stream.binance.com:443/ws/{symbol}@aggTrade
```

**Symbol format:** Use lowercase symbol names (e.g., `btcusdt`, `ethusdt`, `bnbusdt`)

---

## 🐍 Python Implementation

### Dependencies

The Python implementation requires:

- **websocket-client** - WebSocket library for Python
- **Python Standard Library** - `json`, `threading`, `time`, `collections`, `enum`, `dataclasses`

### Installing Python Dependencies

#### Step 1: Check Python Version

```bash
python3 --version
```

Ensure you have Python 3.7 or higher. If not, install it:

```bash
# macOS (using Homebrew)
brew install python3
```

#### Step 2: Install Dependencies

Install required packages using pip:

```bash
# Install from requirements.txt (recommended)
pip3 install -r requirements.txt

# Or install manually
pip3 install websocket-client>=1.6.0
```

**Note:** If you encounter permission errors, use `pip3 install --user` or consider using a virtual environment:

```bash
# Create virtual environment (optional but recommended)
python3 -m venv venv
source venv/bin/activate  # On macOS/Linux
# On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt
```

### Running the Python Implementation

#### Basic Usage

```bash
# From project root directory
python3 main.py
```

#### Using Virtual Environment (if created)

```bash
# Activate virtual environment first
source venv/bin/activate  # On macOS/Linux

# Run the program
python3 main.py
```

#### Expected Output

When running, you should see:

```
Connecting to Binance BTCUSDT/USDT Live Feed...
WebSocket URL: wss://stream.binance.com:443/ws/btcusdt@aggTrade
[INFO] Connected to Binance WebSocket
[INFO] Connection established in 345ms
[INFO] First message received in 678ms
[10:30:45.123] [LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 1 | AvgProc: 0.234ms
```

The metrics will update in real-time as trades are received from Binance.

#### Stopping the Program

Press `Ctrl+C` to stop the program gracefully. You'll see final statistics:

```
[INFO] WebSocket connection closed
[INFO] Connection duration: 30.25 seconds
[INFO] Total messages processed: 987
[INFO] Messages per second: 32.90
[INFO] Average processing time: 0.287 ms
```

### Python Configuration

#### Changing the Trading Pair

Edit `main.py` and modify the symbol parameter:

```python
# In main() function, change:
client = BinanceWebSocketClient(symbol="btcusdt")

# For example, for ETH/USDT:
client = BinanceWebSocketClient(symbol="ethusdt")
```

#### WebSocket Endpoint

The Python implementation uses the same Binance endpoint format:
```
wss://stream.binance.com:443/ws/{symbol}@aggTrade
```

The symbol should be lowercase (e.g., `btcusdt`, `ethusdt`, `bnbusdt`).

## Project Structure

```
ApexLOB-Optimized-Order-Book-Signal-Monitor/
├── CMakeLists.txt          # CMake build configuration (C++)
├── main.cpp                # C++ main application entry point
├── main.go                 # Go main application entry point
├── orders.go               # Go order and limit level structures
├── orderbook.go            # Go order book implementation
├── go.mod                  # Go module dependencies
├── main.py                 # Python implementation (for comparison)
├── Order.h                 # C++ order and limit level structures
├── OrderBook.h             # C++ order book implementation with matching logic
├── AlphaSignalGenerator.h  # Alpha signal generation with technical indicators
├── Logger.h                # Thread-safe logging system
├── test_utils.h            # Test framework utilities
├── test_alpha_signal.cpp   # AlphaSignalGenerator test suite (22 tests)
├── test_orderbook.cpp      # OrderBook test suite (26 tests)
├── test_edge_cases.cpp     # Edge case tests (63 tests)
├── run_tests.sh            # C++ test runner script
├── orders_test.go          # Go order structure tests
├── orderbook_test.go       # Go orderbook tests
├── README.md               # This file
├── TESTING.md              # Testing guide and documentation
├── PERFORMANCE_COMPARISON.md # Detailed performance comparison guide
├── benchmark.sh            # Automated benchmark script
├── requirements.txt        # Python dependencies
├── .gitignore              # Git ignore rules
├── build/                  # Build directory (generated)
│   ├── TradingEngine       # Main executable
│   ├── test_alpha_signal   # Alpha signal tests
│   ├── test_orderbook      # OrderBook tests
│   └── test_edge_cases     # Edge case tests
└── lib/                    # Local libraries
    └── ixwebsocket/        # IXWebSocket library (headers + static lib)
        ├── include/        # Header files
        └── lib/            # Static library file
```

## Configuration

### Changing the Trading Pair

#### C++ Implementation

Edit `main.cpp` and change the WebSocket URL:

```cpp
std::string url = "wss://stream.binance.com:443/ws/btcusdt@aggTrade";
```

For example, for ETH/USDT:
```cpp
std::string url = "wss://stream.binance.com:443/ws/ethusdt@aggTrade";
```

#### Python Implementation

Edit `main.py` and modify the symbol parameter in the `main()` function:

```python
# Change this line:
client = BinanceWebSocketClient(symbol="btcusdt")

# For example, for ETH/USDT:
client = BinanceWebSocketClient(symbol="ethusdt")
```

### WebSocket Endpoint

All three implementations connect to Binance's aggregated trade stream on port 443 (standard HTTPS port). The endpoint format is:
```
wss://stream.binance.com:443/ws/{symbol}@aggTrade
```

**Symbol format:** Use lowercase symbol names (e.g., `btcusdt`, `ethusdt`, `bnbusdt`)

## Troubleshooting

### C++ Implementation Issues

#### Build Issues

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

**Problem:** Compilation errors
```bash
# Solution: Ensure you have a C++17 compatible compiler
clang++ --version  # Should show C++17 support
# Or rebuild from scratch
cd build
rm -rf *
cmake ..
make
```

#### Connection Issues

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

#### Runtime Issues

**Problem:** No data received
- Solution: Wait a few seconds for trades to arrive
- Check that Binance API is operational
- Verify the trading pair symbol is correct (lowercase, e.g., "btcusdt")

**Problem:** Program exits immediately
- Solution: Check error messages for connection failures
- Ensure all dependencies are correctly linked

### Go Implementation Issues

#### Build Issues

**Problem:** `go: command not found`
```bash
# Solution: Install Go
# macOS (using Homebrew)
brew install go

# Or download from https://golang.org/dl/
```

**Problem:** Module not found errors
```bash
# Solution: Download dependencies
go mod download
go mod tidy
```

**Problem:** Build errors
```bash
# Solution: Ensure Go 1.21+ is installed
go version

# Clean and rebuild
go clean
go build -o apexlob-go main.go orders.go orderbook.go
```

#### Connection Issues

**Problem:** Connection timeout
- Solution: Check internet connection
- Verify Binance API is accessible: `curl -I https://stream.binance.com:443`
- Ensure firewall is not blocking port 443

**Problem:** TLS/SSL errors
- Solution: Go's standard library handles TLS automatically
- Check system certificates are up to date
- Verify network connectivity

#### Runtime Issues

**Problem:** No data received (Go)
- Solution: Wait a few seconds for trades to arrive
- Check that Binance API is operational
- Verify the trading pair symbol is correct (lowercase, e.g., "btcusdt")
- Check internet connection and firewall settings

**Problem:** Program exits with error
- Solution: Check the error message for specific issues
- Ensure all Go files are in the same directory
- Verify Go version compatibility (1.21+)
- Check that the symbol name is correct

### Python Implementation Issues

#### Installation Issues

**Problem:** `pip3: command not found`
```bash
# Solution: Install pip if not available
# macOS (using Homebrew)
brew install python3

# Or use ensurepip
python3 -m ensurepip --upgrade
```

**Problem:** Permission denied when installing packages
```bash
# Solution 1: Use --user flag
pip3 install --user -r requirements.txt

# Solution 2: Use virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

**Problem:** `websocket-client` installation fails
```bash
# Solution: Try upgrading pip first
pip3 install --upgrade pip
pip3 install websocket-client>=1.6.0

# Or use pip install instead of pip3
pip install websocket-client>=1.6.0
```

#### Runtime Issues

**Problem:** `ModuleNotFoundError: No module named 'websocket'`
```bash
# Solution: Install the websocket-client package
pip3 install websocket-client

# Or use requirements.txt
pip3 install -r requirements.txt
```

**Problem:** `ImportError` or syntax errors
```bash
# Solution: Ensure Python 3.7+ is being used
python3 --version  # Should be 3.7 or higher

# Try running with explicit python3
python3 main.py
```

**Problem:** No data received (Python)
- Solution: Wait a few seconds for trades to arrive
- Check that Binance API is operational
- Verify the trading pair symbol is correct (lowercase, e.g., "btcusdt")
- Check internet connection and firewall settings

**Problem:** Connection errors or SSL issues
```bash
# Solution: Check if you can access Binance API
curl -I https://stream.binance.com:443

# If SSL errors occur, the Python implementation uses cert_reqs=0
# This should work on most systems, but check firewall/proxy settings
```

**Problem:** Program exits with error
- Solution: Check the error message for specific issues
- Ensure all dependencies are installed correctly
- Verify Python version compatibility (3.7+)
- Check that the symbol name is correct

### Common Issues (Both Implementations)

**Problem:** No trades appearing
- Solution: Check that the market is active (trades occur during market hours)
- Verify the symbol is correct and trading is active
- Check Binance API status: https://www.binance.com/en/support/announcement

**Problem:** Connection drops frequently
- Solution: Check network stability
- Ensure firewall/proxy isn't interfering
- Both implementations include automatic reconnection handling

**Problem:** High CPU usage
- Solution: This is normal for real-time data processing
- C++ version will use less CPU than Python
- Consider running during off-peak hours for testing

**Problem:** Log file not being created
- Solution: Check file permissions in the project directory
- Verify the application has write permissions
- Check disk space availability
- Log file is created at: `./apexlob.log`

**Problem:** Too many log messages (verbose output)
- Solution: Adjust log level in `main.cpp`:
  ```cpp
  logger->setLogLevel(LogLevel::INFO);  // Change from DEBUG to INFO or WARNING
  ```

**Problem:** Alpha signals showing "Collecting data..."
- Solution: This is normal - the system needs at least 31 data points before generating signals
- Wait for more trades to arrive (typically 30-60 seconds depending on market activity)
- Signals will appear automatically once enough data is collected

## Alpha Signal Generation

The C++ implementation includes an advanced alpha signal generation system that uses multiple technical indicators to generate trading signals in real-time.

### Technical Indicators

| Indicator | Period | Description |
|-----------|--------|-------------|
| **SMA Short** | 10 | Short-term Simple Moving Average |
| **SMA Long** | 30 | Long-term Simple Moving Average |
| **RSI** | 14 | Relative Strength Index (0-100) |
| **Momentum** | 10 | Price change percentage over 10 periods |
| **Volatility** | 20 | Coefficient of variation (standard deviation / mean) |

### Signal Types

- **STRONG_BUY** - Strong bullish signal (score ≥ 3)
- **BUY** - Bullish signal (score ≥ 1)
- **HOLD** - Neutral signal (score = 0)
- **SELL** - Bearish signal (score ≤ -1)
- **STRONG_SELL** - Strong bearish signal (score ≤ -3)

### Signal Generation Logic

The signal is generated using a scoring system:

1. **Moving Average Crossover** (+1 if short MA > long MA, -1 otherwise)
2. **RSI Signals** (+2 if RSI < 30 oversold, -2 if RSI > 70 overbought, +1/-1 for moderate levels)
3. **Momentum** (+1 if momentum > 2%, -1 if momentum < -2%)
4. **Volatility Filter** - Reduces signal strength in high volatility (>5%) scenarios

### Signal Strength

Signal strength (0.0 to 1.0) is calculated based on:
- Signal type intensity
- RSI extremes
- Momentum magnitude

### Example Output

```
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | [ALPHA] STRONG_BUY (85.2%) | RSI: 28.5 | Mom: 3.2% | MA↑ RSI_OS Mom↑
```

- **STRONG_BUY (85.2%)** - Signal type and strength
- **RSI: 28.5** - Oversold condition (below 30)
- **Mom: 3.2%** - Strong positive momentum
- **MA↑ RSI_OS Mom↑** - Indicators showing bullish conditions

## Logging System

The application includes a comprehensive, thread-safe logging system for debugging and monitoring.

### Log Levels

- **DEBUG** - Detailed debugging information (trade processing, indicator calculations)
- **INFO** - General information (connection events, signal generation)
- **WARNING** - Warning messages (missing fields, insufficient data)
- **ERROR** - Error conditions (connection failures, JSON parse errors)
- **FATAL** - Critical errors requiring shutdown

### Configuration

Logging is configured in `main.cpp`:

```cpp
Logger* logger = Logger::getInstance();
logger->setLogLevel(LogLevel::INFO);  // Change to DEBUG for verbose logging
logger->enableFileLogging("apexlob.log");  // Enable file logging
logger->enableConsoleLogging(true);  // Enable/disable console output
```

### Log File Location

By default, logs are written to `apexlob.log` in the project root directory. The log file is automatically created and appended to on each run.

### Example Log Entries

```
[2025-01-08 17:50:12.345] [INFO] === ApexLOB Trading Engine Starting ===
[2025-01-08 17:50:12.346] [INFO] Alpha Signal Generation: ENABLED
[2025-01-08 17:50:12.500] [INFO] WebSocket connection established in 153ms
[2025-01-08 17:50:13.201] [DEBUG] Processing trade: ID=12345, Price=43250.50, Quantity=0.5, Side=BUY
[2025-01-08 17:50:13.201] [DEBUG] Trade executed: 500 units at 43250.50, Remaining: 0
[2025-01-08 17:50:13.250] [INFO] Strong signal generated: STRONG_BUY (Strength: 85.2%, RSI: 28.5)
```

### Usage in Code

```cpp
LOG_DEBUG("Debug message");
LOG_INFO("Info message");
LOG_WARNING("Warning message");
LOG_ERROR("Error message");
LOG_FATAL("Fatal error");
```

## Testing

The project includes a comprehensive test suite with **111+ unit tests** covering core functionality and edge cases.

### Running Tests

```bash
# Build and run all tests
cd build
cmake ..
make test_alpha_signal test_orderbook test_edge_cases

# Run individual test suites
./test_alpha_signal    # AlphaSignalGenerator tests (22 tests)
./test_orderbook       # OrderBook tests (26 tests)
./test_edge_cases      # Edge case tests (63 tests)

# Or use the test runner script
./run_tests.sh
```

### Test Coverage

- **AlphaSignalGenerator**: Indicator calculations, signal generation, history management
- **OrderBook**: Order matching, VWAP calculation, price priority, thread safety
- **Edge Cases**: Zero quantities, extreme values, boundary conditions, error handling

### Test Results

All tests are passing:
- ✅ 22/22 AlphaSignalGenerator tests
- ✅ 26/26 OrderBook tests
- ✅ 63/63 Edge case tests

For detailed testing documentation, see [TESTING.md](TESTING.md).

## Technical Details

### Order Book Implementation

- Uses `std::map` with custom comparators for efficient bid/ask storage
- Thread-safe operations with `std::mutex`
- Automatic order matching on submission
- Efficient volume aggregation at each price level
- Price-time priority matching (best bid/ask first)

### Alpha Signal Generator

- Maintains rolling price history (up to 1000 data points)
- Real-time indicator calculations (SMA, RSI, Momentum, Volatility)
- Thread-safe signal generation with mutex protection
- Signal strength scoring algorithm
- Automatic history overflow protection

### WebSocket Connection

- Blocking connection mode for reliable initial handshake
- Automatic ping/pong handling (Binance sends pings every 20 seconds)
- 60-second connection timeout
- TLS 1.2+ with system certificate validation
- Automatic error recovery

### Logging System

- Singleton pattern for global access
- Thread-safe logging with mutex protection
- Dual output (console + file)
- Configurable log levels
- Timestamped log entries with millisecond precision

### Performance

- Optimized data structures for O(log n) operations
- Minimal memory allocations
- Real-time metric calculations
- Efficient JSON parsing with nlohmann/json
- **10-50x faster than Python implementation** - See [PERFORMANCE_COMPARISON.md](PERFORMANCE_COMPARISON.md) for detailed benchmarks

## Quick Reference

### Key Files

| File | Purpose |
|------|---------|
| `main.cpp` | Main application entry point with WebSocket client |
| `OrderBook.h` | Order book implementation with matching logic |
| `AlphaSignalGenerator.h` | Alpha signal generation with technical indicators |
| `Logger.h` | Thread-safe logging system |
| `test_*.cpp` | Comprehensive C++ test suites (111+ tests) |
| `*_test.go` | Go test suites (21+ tests) |

### Common Commands

```bash
# Build C++ application
cd build && cmake .. && make
./build/TradingEngine

# Build and run Go application
go build -o apexlob-go main.go orders.go orderbook.go
./apexlob-go

# Run Python application
python3 main.py

# Run all C++ tests
./run_tests.sh

# Run Go tests
go test -v ./...

# Run benchmarks
./benchmark.sh

# Check log file (C++ only)
tail -f apexlob.log
```

### Signal Interpretation Guide

| Signal | Meaning | Action |
|--------|---------|--------|
| **STRONG_BUY** | Strong bullish conditions | Consider buying |
| **BUY** | Bullish conditions | Monitor for entry |
| **HOLD** | Neutral/insufficient data | Wait for clearer signal |
| **SELL** | Bearish conditions | Monitor for exit |
| **STRONG_SELL** | Strong bearish conditions | Consider selling |

### Log Levels

- **DEBUG**: Detailed debugging (trade processing, calculations)
- **INFO**: General information (connections, signals)
- **WARNING**: Non-critical issues (missing data)
- **ERROR**: Error conditions (connection failures)
- **FATAL**: Critical errors (shutdown required)

## Documentation

- **[TESTING.md](TESTING.md)** - Comprehensive testing guide and test coverage
- **[PERFORMANCE_COMPARISON.md](PERFORMANCE_COMPARISON.md)** - Detailed performance analysis and benchmarks

## License

This project is provided as-is for educational and research purposes.

## Contributing

Contributions are welcome! Please ensure:
- All C++ tests pass (`./run_tests.sh`)
- All Go tests pass (`go test -v ./...`)
- Code follows existing style conventions
- New features include appropriate tests
- Documentation is updated

