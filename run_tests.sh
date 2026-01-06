#!/bin/bash
# Test runner script for ApexLOB

echo "=========================================="
echo "ApexLOB Test Suite"
echo "=========================================="
echo ""

# Build tests
echo "Building tests..."
cd build
cmake .. > /dev/null 2>&1
make test_alpha_signal test_orderbook test_edge_cases

if [ $? -ne 0 ]; then
    echo "Error: Failed to build tests"
    exit 1
fi

echo ""
echo "=========================================="
echo "Running AlphaSignalGenerator Tests"
echo "=========================================="
./test_alpha_signal
ALPHA_EXIT=$?

echo ""
echo "=========================================="
echo "Running OrderBook Tests"
echo "=========================================="
./test_orderbook
ORDERBOOK_EXIT=$?

echo ""
echo "=========================================="
echo "Running Edge Case Tests"
echo "=========================================="
./test_edge_cases
EDGE_EXIT=$?

echo ""
echo "=========================================="
echo "Test Summary"
echo "=========================================="

if [ $ALPHA_EXIT -eq 0 ] && [ $ORDERBOOK_EXIT -eq 0 ] && [ $EDGE_EXIT -eq 0 ]; then
    echo "All tests passed! ✓"
    exit 0
else
    echo "Some tests failed. ✗"
    exit 1
fi

