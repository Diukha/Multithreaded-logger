#include "FileLoggerTester.h"


FileLoggerTester::FileLoggerTester(FileLogger& _fileLogger): fileLogger(_fileLogger) {
    isLogging = true;
}


void FileLoggerTester::run() {
    std::thread fileLoggerThread(&FileLoggerTester::logMessagesWorker, this);

    while (processUserInput())
        continue;

    {
        std::lock_guard<std::mutex> lock(mutex);
        isLogging = false;
    }

    conditionVariable.notify_one();
    fileLoggerThread.join();
}

std::string FileLoggerTester::getUserInput() {
    std::string userInput = "";
    if (std::getline(std::cin, userInput))
        return userInput;

    throw std::runtime_error("\nОшибка ввода с std::cin\n");
}


bool FileLoggerTester::getLogLevelFromUserInput(LogLevel& logLevel) {
    std::string userInput = "";

    while (true) {
        std::cout << "\nВведите уровень важности сообщения: ";

        try {
            userInput = getUserInput();

            if (userInput.empty())
                logLevel = fileLogger.getDefaultLogLevel();
            else
                logLevel = stringToLogLevel(userInput);

            std::cout << "Вы ввели: " << logLevelToString(logLevel) << "\n";

            break;
        } catch (const std::runtime_error& err) {
            std::cerr << "Произошла ошибка ввода\n";
            return false;
        } catch (const std::invalid_argument& err) {
            std::cerr << "Введён несуществующий уровень логирования: "
                      << userInput
                      << "\n";
            continue;
        }
    }

    return true;
}


bool FileLoggerTester::getMessageTextFromUserInput(std::string& messageText) {
    std::cout << "\nВведите сообщение: ";

    try {
        std::string userInput = getUserInput();

        if (userInput.empty()) return false;

        messageText = userInput;

        std::cout << "Вы ввели: " << userInput << "\n";

    } catch (const std::runtime_error& err) {
        std::cerr << "Произошла ошибка ввода\n";
        return false;
    }

    return true;
}


bool FileLoggerTester::processUserInput() {
    std::string messageText;
    LogLevel currentLogLevel;


    if (!getLogLevelFromUserInput(currentLogLevel))
        return false;


    if (!getMessageTextFromUserInput(messageText))
        return false;

    {
        std::lock_guard<std::mutex> lock(mutex);
        logMessagesQueue.push({messageText, currentLogLevel});
    }

    conditionVariable.notify_one();

    return true;
}


void FileLoggerTester::logMessagesWorker() {
    while (true) {
        LogMessage logMessage;

        {
            std::unique_lock<std::mutex> lock(mutex);
            conditionVariable.wait(
                    lock, 
                    [this] {
                        return !logMessagesQueue.empty() || !isLogging;
                    });

            if (!isLogging && logMessagesQueue.empty())
                break;

            logMessage = logMessagesQueue.front();
            logMessagesQueue.pop();
        }
        
        try {
            fileLogger.log(logMessage.text, logMessage.logLevel);
        } catch (const std::ios_base::failure& e) {
            int systemErrno = errno;
            std::cerr << "\nОшибка: не удалось записать сообщение в журнал\n";

            switch (systemErrno) {
                case ENOSPC:
                    std::cerr << "Причина: недостаточно места на диске\n";
                    break;
                case EACCES:
                case EPERM:
                    std::cerr << "Причина: отказано в доступе к файлу журнала\n";
                    break;
                case EIO:
                    std::cerr << "Причина: аппаратный сбой ввода-вывода\n";
                    break;
                case EFBIG:
                    std::cerr << "Причина: файл журнала превысил допустимый размер\n";
                    break;
                default:
                    std::cerr << "Причина: неизвестная ошибка " << systemErrno << "\n";
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "\nПроизошла критическая ошибка: " << e.what() << "\n";
        }
    }
}
