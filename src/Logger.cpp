#include "Logger.h"
#include <iostream>
#include <stdexcept>


std::string logLevelToString(LogLevel logLevel) {
    switch (logLevel) {
        case LogLevel::INFO:        return "INFO";
        case LogLevel::WARNING:     return "WARNING";
        case LogLevel::ERROR:       return "ERROR";
        default:                    return "UNKNOWN";
    }
}

LogLevel stringToLogLevel(std::string_view logLevel) {
    if (logLevel == "INFO")                             return LogLevel::INFO;
    if (logLevel == "WARNING" || logLevel == "WARN")    return LogLevel::WARNING;
    if (logLevel == "ERROR" || logLevel == "ERR")       return LogLevel::ERROR;
    
    throw std::invalid_argument("Неизвестный уровень логирования: " + std::string(logLevel));
}

FileLogger::FileLogger(const std::string& filename, LogLevel logLevel)
    : file(filename, std::ios::out | std::ios::app),
      defaultLogLevel(logLevel)
{
    if (!file.is_open())
        throw std::runtime_error("Не удалось открыть файл: " + filename);
}


void FileLogger::setDefaultLogLevel(LogLevel logLevel) {
    defaultLogLevel = logLevel;
}


LogLevel FileLogger::getDefaultLogLevel() const {
    return defaultLogLevel;
}


void FileLogger::log(const std::string& message, LogLevel logLevel) {
    if (logLevel >= defaultLogLevel) {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTime = *std::localtime(&currentTime);

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;

        file << std::put_time(&localTime, "%H:%M:%S")
             << "." << std::setfill('0') << std::setw(3) << milliseconds.count()
             << " [" << logLevelToString(logLevel) << "]" << "\t"
             << message << "\n";
    }
}


FileLogger::~FileLogger() {
    if (file.is_open())
        file.close();
}
