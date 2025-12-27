# Performance Comparison Guide

This guide explains how to compare the performance of the C++ and Python implementations of ApexLOB.

## Quick Comparison Methods

### Method 1: Automated Benchmark Script (Recommended)

Run the automated benchmark script that will run both implementations for a fixed duration and compare results:

```bash
./benchmark.sh
```

This script will:
- Run both implementations for 30 seconds (default)
- Collect performance metrics automatically
- Display a comparison summary
- Save detailed logs to `/tmp/cpp_benchmark.log` and `/tmp/python_benchmark.log`

**Note:** You may need to install `timeout` and `bc` commands:
```bash
# macOS - timeout comes with GNU coreutils
brew install coreutils
brew install bc
```

### Method 2: Manual Comparison

1. **Open two terminal windows**

2. **Terminal 1 - Run C++ version:**
   ```bash
   ./build/TradingEngine
   ```

3. **Terminal 2 - Run Python version:**
   ```bash
   python3 main.py
   ```

4. **Let both run for the same duration** (e.g., 1-2 minutes)

5. **Press Ctrl+C to stop both**

6. **Compare the final statistics:**
   - Look at the "Total messages processed" line
   - Look at the "Messages per second" line
   - Look at the "Average processing time" line

## Metrics to Compare

### 1. Connection Time
- **What it measures:** Time from program start to first message received
- **C++:** Usually 200-500ms
- **Python:** Usually 300-800ms
- **Expected:** C++ should be faster due to lower overhead

### 2. Average Processing Time
- **What it measures:** Average time to process each trade message
- **C++:** Typically 0.01-0.1ms per message
- **Python:** Typically 0.1-1.0ms per message
- **Expected:** C++ should be 5-50x faster per message

### 3. Messages Per Second
- **What it measures:** Throughput of processed messages
- **C++:** Typically 100-500+ messages/second (depends on trade frequency)
- **Python:** Typically 50-300 messages/second
- **Expected:** C++ should handle higher throughput

### 4. Memory Usage
- **What it measures:** RAM consumption during operation
- **How to check:**
  ```bash
  # While running, in another terminal:
  # macOS
  top -pid $(pgrep TradingEngine)
  top -pid $(pgrep -f "python3 main.py")
  
  # Or use Activity Monitor
  ```

## Expected Performance Differences

### C++ Advantages:
- ✅ **Faster processing** (10-50x faster per message)
- ✅ **Lower memory usage**
- ✅ **More consistent latency**
- ✅ **Better for high-frequency trading**

### Python Advantages:
- ✅ **Easier to develop and modify**
- ✅ **Better error messages and debugging**
- ✅ **Faster development cycle**
- ✅ **More libraries available**

## Sample Output Comparison

### C++ Output:
```
[INFO] Connection established in 234ms
[INFO] First message received in 567ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.045ms
...
[INFO] Total messages processed: 1245
[INFO] Messages per second: 41.50
[INFO] Average processing time: 0.052 ms
```

### Python Output:
```
[INFO] Connection established in 345ms
[INFO] First message received in 678ms
[LOB] Last: 43250.50 | VWAP: 43248.25 | Vol: 15234 | Msg: 150 | AvgProc: 0.234ms
...
[INFO] Total messages processed: 987
[INFO] Messages per second: 32.90
[INFO] Average processing time: 0.287 ms
```

### Interpretation:
- C++ processed **1.26x more messages** (1245 vs 987)
- C++ is **5.5x faster** per message (0.052ms vs 0.287ms)
- C++ has **26% higher throughput** (41.5 vs 32.9 msg/sec)

## Troubleshooting

### If benchmark script doesn't work:
- Install required tools: `brew install coreutils bc`
- Or use Method 2 (manual comparison) instead

### If you see very different results:
- Ensure both are running during similar market activity
- Run multiple times and average the results
- Check system load (other programs may affect performance)

### For accurate comparison:
- Run both implementations at the same time (not sequentially)
- Use the same duration for both
- Ensure stable network connection
- Close other CPU-intensive applications

## Advanced: Profiling

For detailed performance analysis:

### C++ Profiling:
```bash
# Build with debug symbols
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make

# Run with profiling
perf record ./TradingEngine
perf report
```

### Python Profiling:
```bash
# Run with cProfile
python3 -m cProfile -o profile.stats main.py
python3 -m pstats profile.stats
```

