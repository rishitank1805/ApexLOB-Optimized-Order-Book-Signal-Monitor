#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <chrono>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};

class Logger {
private:
    static Logger* instance;
    static std::mutex mutex_;
    
    LogLevel currentLevel;
    bool logToFile;
    bool logToConsole;
    std::ofstream logFile;
    std::string logFilePath;
    
    Logger() : currentLevel(LogLevel::INFO), logToFile(false), logToConsole(true) {}
    
    std::string getCurrentTimeString() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    void writeLog(LogLevel level, const std::string& message) {
        if (level < currentLevel) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string timestamp = getCurrentTimeString();
        std::string levelStr = levelToString(level);
        std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
        
        if (logToConsole) {
            // Color codes for terminal (optional)
            if (level == LogLevel::ERROR || level == LogLevel::FATAL) {
                std::cerr << logMessage << std::endl;
            } else {
                std::cout << logMessage << std::endl;
            }
        }
        
        if (logToFile && logFile.is_open()) {
            logFile << logMessage << std::endl;
            logFile.flush();
        }
    }

public:
    // Singleton pattern
    static Logger* getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance == nullptr) {
            instance = new Logger();
        }
        return instance;
    }
    
    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void setLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentLevel = level;
    }
    
    void enableFileLogging(const std::string& filePath) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile.is_open()) {
            logFile.close();
        }
        logFile.open(filePath, std::ios::app);
        if (logFile.is_open()) {
            logToFile = true;
            logFilePath = filePath;
            writeLog(LogLevel::INFO, "File logging enabled: " + filePath);
        } else {
            writeLog(LogLevel::ERROR, "Failed to open log file: " + filePath);
        }
    }
    
    void disableFileLogging() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile.is_open()) {
            logFile.close();
        }
        logToFile = false;
    }
    
    void enableConsoleLogging(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        logToConsole = enable;
    }
    
    void debug(const std::string& message) {
        writeLog(LogLevel::DEBUG, message);
    }
    
    void info(const std::string& message) {
        writeLog(LogLevel::INFO, message);
    }
    
    void warning(const std::string& message) {
        writeLog(LogLevel::WARNING, message);
    }
    
    void error(const std::string& message) {
        writeLog(LogLevel::ERROR, message);
    }
    
    void fatal(const std::string& message) {
        writeLog(LogLevel::FATAL, message);
    }
    
    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

// Static member definitions
Logger* Logger::instance = nullptr;
std::mutex Logger::mutex_;

// Convenience macros
#define LOG_DEBUG(msg) Logger::getInstance()->debug(msg)
#define LOG_INFO(msg) Logger::getInstance()->info(msg)
#define LOG_WARNING(msg) Logger::getInstance()->warning(msg)
#define LOG_ERROR(msg) Logger::getInstance()->error(msg)
#define LOG_FATAL(msg) Logger::getInstance()->fatal(msg)

#endif

