cmake_minimum_required(VERSION 3.10)
project(ApexLOB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find thread library for the simulator
find_package(Threads REQUIRED)

add_executable(TradingEngine main.cpp)
target_link_libraries(TradingEngine PRIVATE Threads::Threads)