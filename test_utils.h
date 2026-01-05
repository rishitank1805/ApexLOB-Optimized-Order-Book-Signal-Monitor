#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>

// Simple test framework
class TestRunner {
private:
    static int totalTests;
    static int passedTests;
    static int failedTests;
    static std::vector<std::string> failures;

public:
    static void reset() {
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
        failures.clear();
    }

    static void assertTrue(bool condition, const std::string& testName) {
        totalTests++;
        if (condition) {
            passedTests++;
            std::cout << "✓ " << testName << std::endl;
        } else {
            failedTests++;
            failures.push_back(testName);
            std::cerr << "✗ " << testName << " FAILED" << std::endl;
        }
    }

    static void assertFalse(bool condition, const std::string& testName) {
        assertTrue(!condition, testName);
    }

    template<typename T>
    static void assertEquals(const T& expected, const T& actual, const std::string& testName, double tolerance = 0.0001) {
        totalTests++;
        bool passed = false;
        
        if constexpr (std::is_floating_point_v<T>) {
            passed = std::abs(expected - actual) < tolerance;
        } else {
            passed = (expected == actual);
        }
        
        if (passed) {
            passedTests++;
            std::cout << "✓ " << testName << std::endl;
        } else {
            failedTests++;
            failures.push_back(testName);
            std::cerr << "✗ " << testName << " FAILED" << std::endl;
            std::cerr << "  Expected: " << expected << ", Got: " << actual << std::endl;
        }
    }

    static void printSummary() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Test Summary" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        
        if (failedTests > 0) {
            std::cout << "\nFailed Tests:" << std::endl;
            for (const auto& failure : failures) {
                std::cout << "  - " << failure << std::endl;
            }
        }
        
        std::cout << std::string(60, '=') << std::endl;
        
        if (failedTests == 0) {
            std::cout << "All tests passed! ✓" << std::endl;
        } else {
            std::cout << "Some tests failed. ✗" << std::endl;
        }
    }

    static int getExitCode() {
        return (failedTests == 0) ? 0 : 1;
    }
};

// Static member definitions
int TestRunner::totalTests = 0;
int TestRunner::passedTests = 0;
int TestRunner::failedTests = 0;
std::vector<std::string> TestRunner::failures;

#endif

