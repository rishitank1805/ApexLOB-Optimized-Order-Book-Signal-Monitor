# Testing Guide

This document describes the test suite for ApexLOB.

## Test Frameworks

The project uses different test frameworks for different languages:

- **C++**: Custom lightweight test framework (`test_utils.h`)
- **Go**: Standard Go testing package (`testing`)
- **Python**: (No tests currently - can use `unittest` or `pytest`)

### C++ Test Framework

The C++ test framework (`test_utils.h`) provides:
- Simple assertion macros
- Test result tracking
- Detailed failure reporting
- Summary statistics

### Go Test Framework

Go uses the standard `testing` package which provides:
- Built-in test runner (`go test`)
- Benchmarking support (`go test -bench`)
- Coverage analysis (`go test -cover`)
- Table-driven tests

## Running Tests

### C++ Tests

#### Build and Run All C++ Tests

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

### Go Tests

#### Run All Go Tests

```bash
# From project root
go test -v ./...

# Run specific test file
go test -v -run TestOrderBookMatching

# Run with coverage
go test -cover ./...

# Run with detailed coverage report
go test -coverprofile=coverage.out ./...
go tool cover -html=coverage.out
```

#### Run Specific Test Functions

```bash
# Run only OrderBook tests
go test -v -run TestOrderBook

# Run only order structure tests
go test -v -run TestOrder

# Run with benchmark tests
go test -bench=. -benchmem
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

### C++ Tests

#### AlphaSignalGenerator Tests (`test_alpha_signal.cpp`)

**22 test cases** covering:

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

#### OrderBook Tests (`test_orderbook.cpp`)

**26 test cases** covering:

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

#### Edge Case Tests (`test_edge_cases.cpp`)

**63 test cases** covering edge cases and boundary conditions:

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

### Go Tests

#### Order Structure Tests (`orders_test.go`)

**3 test cases** covering:
- Side enum string conversion
- Order creation and initialization
- LimitLevel creation

#### OrderBook Tests (`orderbook_test.go`)

**18 test cases** covering:
1. **Basic Functionality**
   - OrderBook initialization
   - Initial state validation
   - Buy/Sell order submission

2. **Order Matching**
   - Simple order matching
   - Partial order matching
   - Multiple order matches
   - Price priority in matching
   - Orders that don't match
   - Exact quantity matches

3. **Metrics Calculation**
   - VWAP (Volume Weighted Average Price) calculation
   - Total volume tracking
   - Last trade price tracking
   - Cumulative notional tracking

4. **Edge Cases**
   - Zero quantity orders
   - Multiple orders at same price
   - VWAP with zero volume
   - Consecutive trades
   - Remaining quantity after partial match
   - Empty book operations
   - Equal price matching

5. **Concurrency**
   - Concurrent read/write operations
   - Thread safety validation

## Test Results

### Test Statistics

#### C++ Tests
- **AlphaSignalGenerator Tests**: 22/22 passing
- **OrderBook Tests**: 26/26 passing
- **Edge Case Tests**: 63/63 passing
- **C++ Total**: 111/111 tests passing

#### Go Tests
- **Order Structure Tests**: 3/3 passing
- **OrderBook Tests**: 18/18 passing
- **Go Total**: 21/21 tests passing

#### Overall
- **Total Tests**: 132 tests across C++ and Go
- **All Tests Passing**: ✅

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

### C++ Test Structure

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

### Go Test Structure

```go
package main

import "testing"

func TestYourFeature(t *testing.T) {
    obj := NewYourClass()
    
    // Test assertions
    if condition != expected {
        t.Errorf("Test failed: got %v, want %v", condition, expected)
    }
    
    // Table-driven tests (Go best practice)
    tests := []struct {
        name string
        input int
        want int
    }{
        {"test case 1", 1, 2},
        {"test case 2", 2, 4},
    }
    
    for _, tt := range tests {
        t.Run(tt.name, func(t *testing.T) {
            got := YourFunction(tt.input)
            if got != tt.want {
                t.Errorf("YourFunction() = %v, want %v", got, tt.want)
            }
        })
    }
}
```

### Available Assertions

#### C++ Assertions

- `TestRunner::assertTrue(condition, "description")` - Assert condition is true
- `TestRunner::assertFalse(condition, "description")` - Assert condition is false
- `TestRunner::assertEquals(expected, actual, "description", tolerance)` - Assert equality (with optional tolerance for floating point)

#### Go Assertions

Go uses standard testing package assertions:
- `t.Errorf("message")` - Fail test with error message
- `t.Fatalf("message")` - Fail test and stop execution
- `t.Logf("message")` - Log message (only shown on failure)
- Direct comparisons: `if got != want { t.Errorf(...) }`

### Adding Tests to CMakeLists.txt (C++)

```cmake
add_executable(test_your_feature test_your_feature.cpp)
target_include_directories(test_your_feature PRIVATE ${IXWEBSOCKET_INCLUDE_DIR})
add_test(NAME YourFeatureTests COMMAND test_your_feature)
```

### Adding Go Tests

Go tests are automatically discovered. Just create a `*_test.go` file:

```bash
# Create test file
touch your_feature_test.go

# Run tests
go test -v ./...
```

Go automatically finds and runs all `*_test.go` files in the package.

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

- ✅ **C++ AlphaSignalGenerator**: All core functionality tested (22 tests)
- ✅ **C++ OrderBook**: All matching and metric calculations tested (26 tests)
- ✅ **C++ Edge Cases**: Comprehensive edge case and boundary condition testing (63 tests)
- ✅ **Go Order Structures**: Basic structure tests (3 tests)
- ✅ **Go OrderBook**: Core functionality and edge cases tested (18 tests)
- ⚠️ **Integration tests**: Not yet implemented (future work)
- ⚠️ **Performance/Benchmark tests**: Not yet implemented (future work)
- ⚠️ **Python tests**: Not yet implemented (can use unittest or pytest)

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

