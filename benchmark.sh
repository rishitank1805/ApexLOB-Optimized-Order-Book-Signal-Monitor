#!/bin/bash
# Performance Benchmark Script - Compare C++ vs Python implementations

DURATION=30  # Run for 30 seconds by default
CPP_OUTPUT="/tmp/cpp_benchmark.log"
PYTHON_OUTPUT="/tmp/python_benchmark.log"

echo "========================================="
echo "ApexLOB Performance Benchmark"
echo "========================================="
echo ""
echo "This script will run both implementations for $DURATION seconds"
echo "and collect performance metrics for comparison."
echo ""
read -p "Press Enter to start the benchmark, or Ctrl+C to cancel..."
echo ""

# Function to extract metrics from output
extract_metrics() {
    local log_file=$1
    local name=$2
    
    echo ""
    echo "========================================="
    echo "$name Metrics:"
    echo "========================================="
    
    if [ ! -f "$log_file" ]; then
        echo "Log file not found: $log_file"
        return
    fi
    
    # Extract connection time
    connection_time=$(grep "Connection established in" "$log_file" | tail -1 | grep -oP '\d+\.?\d*ms' | head -1 || echo "N/A")
    echo "Connection Time: $connection_time"
    
    # Extract first message time
    first_msg_time=$(grep "First message received in" "$log_file" | tail -1 | grep -oP '\d+\.?\d*ms' | head -1 || echo "N/A")
    echo "First Message Time: $first_msg_time"
    
    # Extract total messages
    total_msgs=$(grep "Total messages processed" "$log_file" | tail -1 | grep -oP '\d+' | head -1 || echo "0")
    echo "Total Messages: $total_msgs"
    
    # Calculate messages per second
    if [ "$total_msgs" != "0" ] && [ "$total_msgs" != "N/A" ]; then
        msgs_per_sec=$(echo "scale=2; $total_msgs / $DURATION" | bc 2>/dev/null || echo "N/A")
        echo "Messages/Second: $msgs_per_sec"
    else
        echo "Messages/Second: N/A"
    fi
    
    # Extract average processing time
    avg_proc=$(grep -oP "AvgProc: \K[\d.]+ms" "$log_file" | tail -1 || echo "N/A")
    echo "Avg Processing Time: $avg_proc"
    
    # Extract final stats
    final_stats=$(grep "\[LOB\]" "$log_file" | tail -1 || echo "N/A")
    echo "Final Stats: $final_stats"
}

# Check if C++ executable exists
if [ ! -f "./build/TradingEngine" ]; then
    echo "Error: C++ executable not found. Building..."
    cd build && cmake .. && make
    cd ..
    if [ ! -f "./build/TradingEngine" ]; then
        echo "Failed to build C++ executable. Exiting."
        exit 1
    fi
fi

# Check if Python dependencies are installed
if ! python3 -c "import websocket" 2>/dev/null; then
    echo "Installing Python dependencies..."
    pip3 install -r requirements.txt
fi

# Check for timeout command (macOS uses gtimeout from coreutils)
TIMEOUT_CMD="timeout"
if ! command -v timeout &> /dev/null; then
    if command -v gtimeout &> /dev/null; then
        TIMEOUT_CMD="gtimeout"
    else
        echo "Warning: timeout command not found. Install with: brew install coreutils"
        echo "Running without timeout (you'll need to Ctrl+C manually)"
        TIMEOUT_CMD=""
    fi
fi

echo "Starting C++ benchmark..."
if [ -n "$TIMEOUT_CMD" ]; then
    ($TIMEOUT_CMD $DURATION ./build/TradingEngine 2>&1 | tee "$CPP_OUTPUT") &
else
    (./build/TradingEngine 2>&1 | tee "$CPP_OUTPUT") &
fi
CPP_PID=$!

sleep 2

echo "Starting Python benchmark..."
if [ -n "$TIMEOUT_CMD" ]; then
    ($TIMEOUT_CMD $DURATION python3 main.py 2>&1 | tee "$PYTHON_OUTPUT") &
else
    (python3 main.py 2>&1 | tee "$PYTHON_OUTPUT") &
fi
PYTHON_PID=$!

echo ""
echo "Running benchmarks for $DURATION seconds..."
echo "Press Ctrl+C to stop early"
echo ""

# Wait for both processes
wait $CPP_PID 2>/dev/null
CPP_EXIT=$?
wait $PYTHON_PID 2>/dev/null
PYTHON_EXIT=$?

sleep 1

echo ""
echo "========================================="
echo "Benchmark Results"
echo "========================================="

extract_metrics "$CPP_OUTPUT" "C++ Implementation"
extract_metrics "$PYTHON_OUTPUT" "Python Implementation"

echo ""
echo "========================================="
echo "Performance Summary"
echo "========================================="

# Extract key metrics for comparison
cpp_msgs=$(grep "Total messages processed" "$CPP_OUTPUT" | tail -1 | grep -oP '\d+' | head -1 || echo "0")
python_msgs=$(grep "Total messages processed" "$PYTHON_OUTPUT" | tail -1 | grep -oP '\d+' | head -1 || echo "0")

cpp_avg=$(grep -oP "AvgProc: \K[\d.]+" "$CPP_OUTPUT" | tail -1 || echo "0")
python_avg=$(grep -oP "AvgProc: \K[\d.]+" "$PYTHON_OUTPUT" | tail -1 || echo "0")

if [ "$cpp_msgs" != "0" ] && [ "$python_msgs" != "0" ] && command -v bc &> /dev/null; then
    speedup=$(echo "scale=2; $python_avg / $cpp_avg" | bc 2>/dev/null)
    echo "Processing Speed: C++ is ${speedup}x faster per message (lower is better)"
    
    throughput_ratio=$(echo "scale=2; $cpp_msgs / $python_msgs" | bc 2>/dev/null)
    echo "Throughput: C++ processed ${throughput_ratio}x more messages"
fi

echo ""
echo "Detailed logs saved to:"
echo "  C++: $CPP_OUTPUT"
echo "  Python: $PYTHON_OUTPUT"
echo ""

