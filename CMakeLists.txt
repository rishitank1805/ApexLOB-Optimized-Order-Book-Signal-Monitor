cmake_minimum_required(VERSION 3.10)
project(ApexLOB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Force CMake to look in Homebrew directories first
set(HOMEBREW_PREFIX "/opt/homebrew")
include_directories(${HOMEBREW_PREFIX}/include)
link_directories(${HOMEBREW_PREFIX}/lib)

# 2. Manually locate the IXWebSocket headers and library
# First try local project directory, then Homebrew
set(LOCAL_IXWEBSOCKET_INCLUDE "${CMAKE_SOURCE_DIR}/lib/ixwebsocket/include")
set(LOCAL_IXWEBSOCKET_LIB "${CMAKE_SOURCE_DIR}/lib/ixwebsocket/lib/libixwebsocket.a")

if(EXISTS "${LOCAL_IXWEBSOCKET_INCLUDE}/ixwebsocket/IXWebSocket.h" AND EXISTS "${LOCAL_IXWEBSOCKET_LIB}")
    set(IXWEBSOCKET_INCLUDE_DIR "${LOCAL_IXWEBSOCKET_INCLUDE}")
    set(IXWEBSOCKET_LIBRARY "${LOCAL_IXWEBSOCKET_LIB}")
    message(STATUS "Using local IXWebSocket library")
else()
    # Fallback to Homebrew
    find_path(IXWEBSOCKET_INCLUDE_DIR 
        NAMES ixwebsocket/IXWebSocket.h
        HINTS ${HOMEBREW_PREFIX}/include
    )
    find_library(IXWEBSOCKET_LIBRARY 
        NAMES ixwebsocket
        HINTS ${HOMEBREW_PREFIX}/lib
    )
endif()

# Debug: This will print the paths to your terminal so you can see if they were found
message(STATUS "IXWebSocket Include: ${IXWEBSOCKET_INCLUDE_DIR}")
message(STATUS "IXWebSocket Library: ${IXWEBSOCKET_LIBRARY}")

if(NOT IXWEBSOCKET_INCLUDE_DIR OR NOT IXWEBSOCKET_LIBRARY)
    message(FATAL_ERROR "IXWebSocket library not found! Please ensure it is installed.")
endif()

# 3. Find other dependencies
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(nlohmann_json REQUIRED)

# 4. Create the executable
add_executable(TradingEngine main.cpp)

# 5. Link everything
target_include_directories(TradingEngine PRIVATE ${IXWEBSOCKET_INCLUDE_DIR})

# Find required frameworks for macOS TLS support (SecureTransport)
if(APPLE)
    find_library(SECURITY_FRAMEWORK Security)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    if(SECURITY_FRAMEWORK)
        message(STATUS "Found Security framework for TLS support")
    endif()
    if(COREFOUNDATION_FRAMEWORK)
        message(STATUS "Found CoreFoundation framework")
    endif()
endif()

target_link_libraries(TradingEngine PRIVATE 
    ${IXWEBSOCKET_LIBRARY}
    nlohmann_json::nlohmann_json
    Threads::Threads 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    ZLIB::ZLIB
)

# Link required frameworks on macOS for SecureTransport TLS
if(APPLE)
    if(SECURITY_FRAMEWORK)
        target_link_libraries(TradingEngine PRIVATE ${SECURITY_FRAMEWORK})
    endif()
    if(COREFOUNDATION_FRAMEWORK)
        target_link_libraries(TradingEngine PRIVATE ${COREFOUNDATION_FRAMEWORK})
    endif()
endif()