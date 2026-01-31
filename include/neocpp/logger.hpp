#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <mutex>

namespace neocpp {

/// Log levels for the logger
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
    OFF = 6
};

/// Simple thread-safe logger for the SDK
class Logger {
private:
    static LogLevel currentLevel_;
    static std::mutex logMutex_;
    static bool colorEnabled_;

    /// Get current timestamp string
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    /// Get color code for log level
    static std::string getColor(LogLevel level) {
        if (!colorEnabled_) return "";
        switch (level) {
            case LogLevel::TRACE: return "\033[37m";  // White
            case LogLevel::DEBUG: return "\033[36m";  // Cyan
            case LogLevel::INFO:  return "\033[32m";  // Green
            case LogLevel::WARN:  return "\033[33m";  // Yellow
            case LogLevel::ERROR: return "\033[31m";  // Red
            case LogLevel::FATAL: return "\033[35m";  // Magenta
            default: return "";
        }
    }

    /// Reset color
    static std::string resetColor() {
        return colorEnabled_ ? "\033[0m" : "";
    }

    /// Get level string
    static std::string getLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO ";
            case LogLevel::WARN:  return "WARN ";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

public:
    /// Set the current log level
    static void setLevel(LogLevel level) {
        currentLevel_ = level;
    }

    /// Get the current log level
    static LogLevel getLevel() {
        return currentLevel_;
    }

    /// Enable/disable colored output
    static void setColorEnabled(bool enabled) {
        colorEnabled_ = enabled;
    }

    /// Log a message at the specified level
    static void log(LogLevel level, const std::string& message, const char* file = nullptr, int line = 0) {
        if (level < currentLevel_) return;

        std::lock_guard<std::mutex> lock(logMutex_);

        std::stringstream ss;
        ss << getColor(level)
           << "[" << getTimestamp() << "] "
           << "[" << getLevelString(level) << "] ";

        if (file && line > 0) {
            // Extract filename from path
            std::string filename(file);
            size_t lastSlash = filename.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                filename = filename.substr(lastSlash + 1);
            }
            ss << "[" << filename << ":" << line << "] ";
        }

        ss << message << resetColor();

        if (level >= LogLevel::ERROR) {
            std::cerr << ss.str() << std::endl;
        } else {
            std::cout << ss.str() << std::endl;
        }
    }

    /// Convenience methods
    static void trace(const std::string& msg, const char* file = nullptr, int line = 0) {
        log(LogLevel::TRACE, msg, file, line);
    }

    static void debug(const std::string& msg, const char* file = nullptr, int line = 0) {
        log(LogLevel::DEBUG, msg, file, line);
    }

    static void info(const std::string& msg, const char* file = nullptr, int line = 0) {
        log(LogLevel::INFO, msg, file, line);
    }

    static void warn(const std::string& msg, const char* file = nullptr, int line = 0) {
        log(LogLevel::WARN, msg, file, line);
    }

    static void error(const std::string& msg, const char* file = nullptr, int line = 0) {
        log(LogLevel::ERROR, msg, file, line);
    }

    static void fatal(const std::string& msg, const char* file = nullptr, int line = 0) {
        log(LogLevel::FATAL, msg, file, line);
    }
};

// Macros for easy logging with file and line information
#define LOG_TRACE(msg) neocpp::Logger::trace(msg, __FILE__, __LINE__)
#define LOG_DEBUG(msg) neocpp::Logger::debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) neocpp::Logger::info(msg, __FILE__, __LINE__)
#define LOG_WARN(msg) neocpp::Logger::warn(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) neocpp::Logger::error(msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) neocpp::Logger::fatal(msg, __FILE__, __LINE__)

} // namespace neocpp
