#pragma once

#include "FileLogger.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <stdexcept>


struct LogMessage {
    std::string text;
    LogLevel logLevel;
};


class FileLoggerTester {
    public:
        FileLoggerTester(FileLogger& _fileLogger);

        void run();

        ~FileLoggerTester() = default;


    private:
        FileLogger& fileLogger;
        std::queue<LogMessage> logMessagesQueue;
        std::mutex mutex;
        std::condition_variable conditionVariable;
        bool isLogging;


        std::string getUserInput();

        bool getLogLevelFromUserInput(LogLevel& logLevel);

        bool getMessageTextFromUserInput(std::string& messageText);

        bool processUserInput();

        void logMessagesWorker();
};
