#include <iostream>
#include "Logger.h"
#include <string_view>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


std::string filename = "";
LogLevel defaultLogLevel = LogLevel::INFO;




struct LogMessage {
    std::string text;
    LogLevel logLevel;
};

std::queue<LogMessage> logMessagesQueue;
std::mutex mutex;
std::condition_variable conditionVariable;
bool finishLogging = false;

void messagesWorker(FileLogger& fileLogger) {
    while (true) {
        LogMessage logMessage;

        {
            std::unique_lock<std::mutex> lock(mutex);
            conditionVariable.wait(
                    lock, 
                    [] {
                        return !logMessagesQueue.empty() || finishLogging;
                    });

            if (finishLogging && logMessagesQueue.empty())
                break;

            logMessage = logMessagesQueue.front();
            logMessagesQueue.pop();
        }
        
        fileLogger.log(logMessage.text, logMessage.logLevel);
    }
}





void printHelp() {
    std::cout
        << "Это приложение для проверки библиотеки записи сообщений в журнал.\n"
        << "Использование: app [ОПЦИЯ] [ЗНАЧЕНИЕ]\n"
        << "Пример использования: app -o output.txt -l ERROR\n"
        << "Доступные опции:\n"
        << "  -o, --output FILE       используется для указания файла журнала\n"
        << "  -l, --log-level LEVEL   используется для указания уровня важности сообщения по умолчанию\n"
        << "  -h, --help              показать эту подсказку\n";
}


bool parseArgs(int argc, char* argv[]) {
    if (argc == 1) {
        printHelp();
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printHelp();
            return false;
        }

        if (arg == "-o" || arg == "--output")
            filename = argv[++i];

        if (arg == "-l" || arg == "--log-level")
            defaultLogLevel = stringToLogLevel(argv[++i]);
    }

    return true;
}

std::string getUserInput() {
    std::string userInput = "";
    if (std::getline(std::cin, userInput))
        return userInput;

    throw std::runtime_error("Ошибка ввода с std::cin");
}

int main(int argc, char* argv[]) {
    if (!parseArgs(argc, argv)) return 1;
    std::cout << "Файл журнала: " << filename << "\n";
    std::cout << "Уровень сообщений по умолчанию: " << logLevelToString(defaultLogLevel) << "\n";

    FileLogger fileLogger = FileLogger(filename, defaultLogLevel);
    std::thread fileLoggerThread(messagesWorker, std::ref(fileLogger));

    LogLevel currentLogLevel = LogLevel::INFO;

    while (true) {
        std::cout << "\nВведите уровень важности сообщения: ";
        std::string userInput = "";

        try {
            userInput = getUserInput();

            if (userInput.empty())
                currentLogLevel = fileLogger.getDefaultLogLevel();
            else
                currentLogLevel = stringToLogLevel(userInput);

            std::cout << "Вы ввели: " << logLevelToString(currentLogLevel);
        } catch (const std::runtime_error& err) {
            std::cout << "Произошла ошибка ввода\n";
            break;
        } catch (const std::invalid_argument& err) {
            std::cout << "Введён несуществующий уровень логирования: "
                      << userInput
                      << "\n";
            continue;
        }

        std::cout << "\nВведите сообщение: ";
        
        try {
            userInput = getUserInput();

            if (userInput.empty()) break;

            std::cout << "Вы ввели: " << userInput << "\n";

            {
                std::lock_guard<std::mutex> lock(mutex);
                logMessagesQueue.push({userInput, currentLogLevel});
            }
            conditionVariable.notify_one();

        } catch (const std::runtime_error& err) {
            std::cout << "Произошла ошибка ввода\n";
            break;
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        finishLogging = true;
    }
    conditionVariable.notify_one();
    fileLoggerThread.join();

    return 0;
}
