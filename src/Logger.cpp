#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void Logger::init(const std::string& logFilePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(logFilePath, std::ios::app | std::ios::out);
    if (!logFile_.is_open()) {
        throw std::runtime_error("Cannot open log file: " + logFilePath);
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string timestamp = currentTimestamp();
    std::string lvl = levelToString(level);
    std::string line = "[" + timestamp + "] [" + lvl + "] " + message;

    if (logFile_.is_open()) {
        logFile_ << line << std::endl;
        logFile_.flush();
    }

    history_.push_back(line);

    if (callback_) {
        callback_(line);
    }
}

const std::vector<std::string>& Logger::getLogHistory() const {
    return history_;
}

void Logger::clearHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    history_.clear();
}

void Logger::setCallback(LogCallback cb) {
    std::lock_guard<std::mutex> lock(mutex_);
    callback_ = std::move(cb);
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERR:     return "ERROR";
    }
    return "UNKNOWN";
}

std::string Logger::currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &timeT);
#else
    localtime_r(&timeT, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
