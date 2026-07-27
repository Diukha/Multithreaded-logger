#include "FileLogger.h"
#include <chrono>
#include <stdexcept>
#include <iomanip>


std::string logLevelToString(LogLevel logLevel) {
    if (logLevel == LogLevel::INFO)     return "INFO";
    if (logLevel == LogLevel::WARNING)  return "WARNING";
    if (logLevel == LogLevel::ERROR)    return "ERROR";
    
    throw std::invalid_argument("Неизвестный уровень логирования.");
}


LogLevel stringToLogLevel(std::string_view logLevel) {
    // Для удобства добавлены алиасы.
    if (logLevel == "INFO")                             return LogLevel::INFO;
    if (logLevel == "WARNING" || logLevel == "WARN")    return LogLevel::WARNING;
    if (logLevel == "ERROR" || logLevel == "ERR")       return LogLevel::ERROR;
    
    throw std::invalid_argument("Неизвестный уровень логирования.");
}


FileLogger::FileLogger(const std::string& fileName, LogLevel defaultLogLevel)
    : defaultLogLevel_(defaultLogLevel) {

    if (fileName.empty())
        throw std::runtime_error("Не указан файл журнала.");

    // Включает сообщения об ошибках с файлом.
    logFile_.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    logFile_.open(fileName, std::ios::out | std::ios::app);
}


void FileLogger::setDefaultLogLevel(LogLevel logLevel) {
    defaultLogLevel_ = logLevel;
}


LogLevel FileLogger::getDefaultLogLevel() const {
    return defaultLogLevel_;
}


void FileLogger::log(const std::string& message, LogLevel logLevel) {
    // Сообщения с уровнем ниже заданного не добавляются в журнал.
    if (logLevel >= defaultLogLevel_) { 
        std::unique_lock<std::mutex> lock(mutex_);

    // ---- Получение времени: часов, минут, секунд, миллисекунд.
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm* localTime = std::localtime(&currentTime);

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;
    // ---------------------------------------------------------

        logFile_ << std::put_time(localTime, "%H:%M:%S")
             << "."
             << std::setfill('0') // дописывает старшие нули
             << std::setw(3) // 3 символа для миллисекунд
             << milliseconds.count()
             << " [" << logLevelToString(logLevel) << "]" << "\t"
             << message << "\n";

        // Сразу записывает сообщение в файл, а не ждёт буфер.
        logFile_.flush();
    }
}


FileLogger::~FileLogger() {
    if (logFile_.is_open())
        logFile_.close();
}
