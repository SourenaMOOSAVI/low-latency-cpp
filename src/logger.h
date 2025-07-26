#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <iostream>

/**
 * @brief Singleton logger for thread-safe logging to file and console.
 * 
 * Provides synchronized logging for low-latency systems, using a mutex to prevent file access conflicts.
 * Falls back to console if file operations fail, ensuring debug output is captured.
 */
class Logger {
public:
    /**
     * @brief Gets the singleton Logger instance.
     * @return Reference to the Logger instance.
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    /**
     * @brief Logs a message to file and optionally to console.
     * @param message The message to log.
     * @param to_console If true, also logs to console.
     * 
     * Uses std::mutex to ensure thread-safe file writes.
     */
    void log(const std::string& message, bool to_console = false) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (log_file_.is_open()) {
            log_file_ << message << "\n";
            log_file_.flush();
        }
        if (to_console || !log_file_.is_open()) {
            std::cout << message << "\n";
            std::cout.flush();
        }
    }

private:
    /**
     * @brief Constructs the Logger, opening the log file.
     * 
     * Opens hft_system.log in append mode. File remains open for performance.
     */
    Logger() {
        log_file_.open("hft_system.log", std::ios::app);
    }

    /**
     * @brief Destructs the Logger, closing the log file.
     */
    ~Logger() {
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

    std::ofstream log_file_; ///< Output file stream for logging.
    std::mutex mutex_;       ///< Mutex for thread-safe logging.

    // Delete copy and move operations for singleton
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
};
