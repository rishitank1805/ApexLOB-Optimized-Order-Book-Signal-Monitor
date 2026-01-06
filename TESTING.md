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
make test_alpha_signal test_orderbook test_edge_cases

# Run AlphaSignalGenerator tests
./test_alpha_signal

# Run OrderBook tests
./test_orderbook

# Run Edge Case tests
./test_edge_cases
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

### Edge Case Tests (`test_edge_cases.cpp`)

**54 test cases** covering edge cases and boundary conditions:

#### AlphaSignalGenerator Edge Cases (11 tests)
- Empty history handling
- Single price point
- Constant prices (no variation)
- Extreme price movements
- RSI boundary conditions (0, 100)
- Very high volatility scenarios
- Rapid price changes
- Zero price handling
- Very large price values
- History overflow protection
- Alternating price patterns

#### OrderBook Edge Cases (13 tests)
- Zero quantity orders
- Multiple orders at same price
- Exact quantity matches
- Very large quantities
- Multiple price level matches
- Various no-match scenarios
- VWAP with zero volume
- Consecutive trades
- Price precision handling
- Empty book operations
- Single-side order book
- Remaining quantity after partial match

## Test Results

### Test Statistics

- **AlphaSignalGenerator Tests**: 21/21 passing
- **OrderBook Tests**: 23/23 passing
- **Edge Case Tests**: 54/54 passing
- **Total**: 98/98 tests passing

### Expected Output

```
========================================
Edge Cases Test Suite
========================================

=== Testing Empty History Edge Cases ===
✓ Empty history should return HOLD
✓ Empty history should have 0 strength
✓ Empty history should have 0 price

...

============================================================
Test Summary
============================================================
Total Tests: 54
Passed: 54
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
- ✅ Edge Cases: Comprehensive edge case and boundary condition testing
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

