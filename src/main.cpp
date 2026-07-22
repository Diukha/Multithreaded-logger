#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>




enum class LogLevel : int {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};

std::string logLevelToString(LogLevel logLevel) {
    switch (logLevel) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}




class FileLogger {
    public:
        FileLogger(const std::string& filename, LogLevel logLevel)
            : file(filename, std::ios::out | std::ios::app),
              defaultLogLevel(logLevel)
        {
            if (!file.is_open())
                throw std::runtime_error("Не удалось открыть файл: " + filename);
        }

        void setDefaultLogLevel(LogLevel logLevel) {
            defaultLogLevel = logLevel;
        }

        LogLevel getDefaultLogLevel() {
            return defaultLogLevel;
        }

        void log(const std::string& message, LogLevel logLevel) {
            if (logLevel >= defaultLogLevel) {
                // время для HH:MM:SS
                auto now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                std::tm localTime = *std::localtime(&currentTime);

                // время для миллисекунд
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()
                ) % 1000;

                file << std::put_time(&localTime, "%H:%M:%S")
                     << "." << std::setfill('0') << std::setw(3) << milliseconds.count()
                     << " [" << logLevelToString(logLevel) << "]" << "\t"
                     << message << "\n";
            }
        }

        ~FileLogger() {
            if (file.is_open())
                file.close();
        }

    private:
        std::ofstream file;
        LogLevel defaultLogLevel;
};




int main() {
    FileLogger logger = FileLogger("output.txt", LogLevel::INFO);
    logger.setDefaultLogLevel(LogLevel::WARNING);
    std::cout << logLevelToString(logger.getDefaultLogLevel());
    logger.log("12345", LogLevel::WARNING);
}
