#!/bin/bash
# Simple script to compare runtime performance between C++ and Python implementations

echo "========================================="
echo "ApexLOB Runtime Comparison"
echo "========================================="
echo ""
echo "This script will help you compare the runtime performance"
echo "between the C++ and Python implementations."
echo ""
echo "Note: Run each implementation separately and observe:"
echo "  - Connection time (time to first message)"
echo "  - Average message processing time"
echo "  - Total messages processed per second"
echo ""
echo "========================================="
echo ""

# Check if C++ executable exists
if [ -f "./build/TradingEngine" ]; then
    echo "✓ C++ executable found"
    echo "  Run: ./build/TradingEngine"
    echo ""
else
    echo "✗ C++ executable not found. Build it first:"
    echo "  cd build && cmake .. && make"
    echo ""
fi

# Check if Python script exists
if [ -f "./main.py" ]; then
    echo "✓ Python script found"
    echo "  Run: python3 main.py"
    echo ""
else
    echo "✗ Python script not found"
    echo ""
fi

# Check Python dependencies
if command -v python3 &> /dev/null; then
    if python3 -c "import websocket" 2>/dev/null; then
        echo "✓ Python websocket-client library installed"
    else
        echo "⚠ Python websocket-client library not installed"
        echo "  Install with: pip3 install -r requirements.txt"
    fi
    echo ""
fi

echo "========================================="
echo "Performance Metrics to Compare:"
echo "========================================="
echo "1. Connection Time - Time from start to first message"
echo "2. Avg Processing Time - Average time to process each message"
echo "3. Messages/Second - Throughput of processed messages"
echo "4. Memory Usage - Use 'top' or Activity Monitor while running"
echo ""
echo "Press Ctrl+C in each terminal to stop after collecting data"
echo ""

