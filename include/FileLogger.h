#pragma once

#include <string>
#include <fstream>
#include <string_view>
#include <atomic>
#include <mutex>


// Все возможные уровни важности сообщений
enum class LogLevel : int {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};


// Преобразует уровень важности сообщений в строку.
// Например, LogLevel::INFO преобразуется в "INFO".
std::string logLevelToString(LogLevel logLevel);


// Преобразует строковое представление важности сообщений
// в элемент перечисления.
// Например, "ERROR" преобразуется в LogLevel::ERROR.
LogLevel stringToLogLevel(std::string_view logLevel);


class FileLogger {
    public:
        // При инициализации открывает файл журнала в режиме дозаписи.
        FileLogger(const std::string& fileName, LogLevel defaultLogLevel);

        void setDefaultLogLevel(LogLevel logLevel);

        LogLevel getDefaultLogLevel() const;

        // Записывает в журнал logFile_ сообщение в формате:
        // Ч:М:С.МС [УРОВЕНЬ_ВАЖНОСТИ]  СООБЩЕНИЕ
        // Например: 18:44:26.123 [INFO]   hello world!
        // Время записи сообщения вычисляется перед самой записью внутри метода.
        void log(const std::string& message, LogLevel logLevel);

        // Закрывает файл журнала.
        ~FileLogger();

    private:
        std::ofstream logFile_; // файл журнала
        std::atomic<LogLevel> defaultLogLevel_; // уровень важности по умолчанию
        std::mutex mutex_; // нужен для потокобезопасной записи в файл
};
