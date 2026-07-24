#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string_view>


enum class LogLevel : int {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};


std::string logLevelToString(LogLevel logLevel);
LogLevel stringToLogLevel(std::string_view logLevel);


class FileLogger {
    public:
        FileLogger(const std::string& filename, LogLevel logLevel);
        void setDefaultLogLevel(LogLevel logLevel);
        LogLevel getDefaultLogLevel() const;
        void log(const std::string& message, LogLevel logLevel);
        ~FileLogger();

    private:
        std::ofstream file;
        LogLevel defaultLogLevel;
};

