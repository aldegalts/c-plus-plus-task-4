#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <functional>

enum class LogLevel {
    INFO,
    WARNING,
    ERR
};

class Logger {
public:
    using LogCallback = std::function<void(const std::string&)>;

    static Logger& instance();

    void init(const std::string& logFilePath);
    void log(LogLevel level, const std::string& message);

    const std::vector<std::string>& getLogHistory() const;
    void clearHistory();

    void setCallback(LogCallback cb);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    ~Logger();

    std::ofstream logFile_;
    std::vector<std::string> history_;
    LogCallback callback_;
    mutable std::mutex mutex_;

    static std::string levelToString(LogLevel level);
    static std::string currentTimestamp();
};

#endif
