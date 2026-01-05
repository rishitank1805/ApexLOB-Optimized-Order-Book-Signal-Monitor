# Testing Guide

This document describes the test suite for ApexLOB.

## Test Framework

The project uses a custom lightweight test framework (`test_utils.h`) that provides:
- Simple assertion macros
- Test result tracking
- Detailed failure reporting
- Summary statistics

## Running Tests

### Build and Run All Tests

```bash
# From project root
cd build
cmake ..
make test_alpha_signal test_orderbook

# Run AlphaSignalGenerator tests
./test_alpha_signal

# Run OrderBook tests
./test_orderbook
```

### Using the Test Runner Script

```bash
# Make script executable (if needed)
chmod +x run_tests.sh

# Run all tests
./run_tests.sh
```

### Using CMake Test Framework

```bash
cd build
cmake ..
make
ctest --verbose
```

## Test Coverage

### AlphaSignalGenerator Tests (`test_alpha_signal.cpp`)

**21 test cases** covering:

1. **Basic Functionality**
   - Initial state validation
   - Signal generation with insufficient data

2. **Technical Indicators**
   - Simple Moving Average (SMA) calculation
   - Relative Strength Index (RSI) calculation
   - Momentum calculation
   - Volatility calculation

3. **Signal Generation**
   - STRONG_BUY signal generation
   - STRONG_SELL signal generation
   - Signal strength calculation

4. **Data Management**
   - History size limits
   - Price history maintenance

5. **Utilities**
   - Signal type to string conversion

### OrderBook Tests (`test_orderbook.cpp`)

**23 test cases** covering:

1. **Basic Functionality**
   - Initial state validation
   - Order submission (buy/sell)

2. **Order Matching**
   - Simple order matching
   - Partial order matching
   - Multiple order matches
   - Price priority in matching
   - Orders that don't match

3. **Metrics Calculation**
   - VWAP (Volume Weighted Average Price) calculation
   - Total volume tracking
   - Last trade price tracking

4. **Thread Safety**
   - Concurrent read operations
   - Metric consistency

## Test Results

### Expected Output

```
========================================
AlphaSignalGenerator Test Suite
========================================

=== Testing AlphaSignalGenerator Basic Functionality ===
✓ Initial history size should be 0
✓ Signal should be HOLD with no data
✓ Signal strength should be 0 with no data

...

============================================================
Test Summary
============================================================
Total Tests: 21
Passed: 21
Failed: 0
============================================================
All tests passed! ✓
```

## Writing New Tests

### Test Structure

```cpp
#include "test_utils.h"
#include "YourClass.h"

void testYourFeature() {
    std::cout << "\n=== Testing Your Feature ===" << std::endl;
    
    YourClass obj;
    
    // Test assertions
    TestRunner::assertTrue(condition, "Test description");
    TestRunner::assertEquals(expected, actual, "Test description");
    TestRunner::assertFalse(condition, "Test description");
}

int main() {
    TestRunner::reset();
    
    testYourFeature();
    
    TestRunner::printSummary();
    return TestRunner::getExitCode();
}
```

### Available Assertions

- `TestRunner::assertTrue(condition, "description")` - Assert condition is true
- `TestRunner::assertFalse(condition, "description")` - Assert condition is false
- `TestRunner::assertEquals(expected, actual, "description", tolerance)` - Assert equality (with optional tolerance for floating point)

### Adding Tests to CMakeLists.txt

```cmake
add_executable(test_your_feature test_your_feature.cpp)
target_include_directories(test_your_feature PRIVATE ${IXWEBSOCKET_INCLUDE_DIR})
add_test(NAME YourFeatureTests COMMAND test_your_feature)
```

## Continuous Integration

Tests can be integrated into CI/CD pipelines:

```bash
#!/bin/bash
set -e

cd build
cmake ..
make test_alpha_signal test_orderbook
./test_alpha_signal
./test_orderbook

# Exit with error if any test fails
if [ $? -ne 0 ]; then
    exit 1
fi
```

## Test Coverage Goals

- ✅ AlphaSignalGenerator: All core functionality tested
- ✅ OrderBook: All matching and metric calculations tested
- ⚠️ Integration tests: Not yet implemented (future work)
- ⚠️ Performance tests: Not yet implemented (future work)

## Troubleshooting

### Tests Fail to Compile

- Ensure all dependencies are installed
- Check that CMake configuration is correct
- Verify C++17 standard is enabled

### Tests Fail at Runtime

- Check test output for specific failure messages
- Verify test data matches expected behavior
- Review implementation for bugs

### Floating Point Precision Issues

Use tolerance parameter in `assertEquals`:

```cpp
TestRunner::assertEquals(expected, actual, "description", 0.001); // 0.001 tolerance
```

## Future Test Enhancements

- [ ] Integration tests for full trading flow
- [ ] Performance/benchmark tests
- [ ] Stress tests for high message volumes
- [ ] Memory leak detection tests
- [ ] Multi-threaded concurrency tests

