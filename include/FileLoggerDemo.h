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


class FileLoggerDemo {
    public:
        FileLoggerDemo(FileLogger& fileLogger);

        // Создаёт поток в котором запускает непрерывную обработку ввода
        // пользователя processUserInput().
        void run();

        ~FileLoggerDemo() = default;

    private:
        FileLogger& fileLogger_;
        std::queue<LogMessage> logMessagesQueue_; // сюда поступают все сообщения
        std::mutex mutex_; // нужен для потокобезопасной очереди
        bool isLogging_;

        // Уведомляет worker о новом сообщении, или о прекращении логирования
        std::condition_variable conditionVariable_; 


        // Запрашивает ввод пользователя через std::cin
        std::string getUserInput();

        // Использует getUserInput(), чтобы получить уровень важности
        // сообщения от пользователя.
        // Возвращает true при успешном выполнении и false - при ошибке.
        bool getLogLevelFromUserInput(LogLevel& logLevel);

        // Использует getUserInput(), чтобы получить текст сообщения
        // от пользователя.
        // Возвращает true при успешном выполнении и false - при ошибке.
        bool getMessageTextFromUserInput(std::string& messageText);

        // Единожды получает сообщение типа LogMessage от пользователя
        // и складывает его в очередь logMessagesQueue_.
        bool processUserInput();

        // Если в очереди logMessagesQueue_ накапливаются сообщения,
        // записывает их в журнал через fileLogger_.log(). 
        void logMessagesWorker();
};
