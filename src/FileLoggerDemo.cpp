#include "FileLoggerDemo.h"


FileLoggerDemo::FileLoggerDemo(FileLogger& fileLogger)
    : fileLogger_(fileLogger) {

    isLogging_ = true; // обработка ввода и запись в журнал работают, пока true
}


void FileLoggerDemo::run() {
    // Создаётся отдельный поток для записи сообщений в журнал.
    std::thread fileLoggerThread(&FileLoggerDemo::logMessagesWorker, this);

    while (processUserInput())
        continue;

    { // если дошли до сюда, значит воркер уснул, и завершать поток безопасно
        std::lock_guard<std::mutex> lock(mutex_);
        isLogging_ = false;
    }

    // Это за скобками, чтобы воркер не пытался захватить mutex_,
    // пока воркера будят.
    conditionVariable_.notify_one();

    fileLoggerThread.join();
}


std::string FileLoggerDemo::getUserInput() {
    std::string userInput = "";
    if (std::getline(std::cin, userInput))
        return userInput;

    throw std::runtime_error("\nОшибка ввода с std::cin.\n");
}


bool FileLoggerDemo::getLogLevelFromUserInput(LogLevel& logLevel) {
    std::string userInput = "";

    // Пока пользователь не введёт существующий уровень важности, 
    // продолжает опрашивать пользователя.
    while (true) {
        std::cout << "\nВведите уровень важности сообщения: ";

        try {
            userInput = getUserInput();

            if (userInput.empty())
                // Быстрый выбор минимально допустимого
                // уровня важности при пустом вводе.
                logLevel = fileLogger_.getDefaultLogLevel();
            else
                logLevel = stringToLogLevel(userInput);

            std::cout << "Вы ввели: " << logLevelToString(logLevel) << "\n";

            break;

        } catch (const std::runtime_error& err) {
            std::cerr << "Произошла ошибка ввода\n";
            return false;
        } catch (const std::invalid_argument& err) {
            std::cerr << "Введён несуществующий уровень важности: "
                      << userInput
                      << "\n";

            // При вводе несуществующего уровня,
            // спрашивает пользователя заново.
            continue;
        }
    }

    return true; // ввод уровня важности успешен
}


bool FileLoggerDemo::getMessageTextFromUserInput(std::string& messageText) {
    std::cout << "\nВведите сообщение: ";

    try {
        std::string userInput = getUserInput();

        // Выход из приложения, если сообщение пустое.
        if (userInput.empty()) return false;

        messageText = userInput;

        std::cout << "Вы ввели: " << userInput << "\n";

    } catch (const std::runtime_error& err) {
        std::cerr << "Произошла ошибка ввода\n";
        return false;
    }

    return true; // ввод сообщения успешен
}


bool FileLoggerDemo::processUserInput() {
    std::string messageText;
    LogLevel currentLogLevel;

    // При ошибке ввода уровня важности,
    if (!getLogLevelFromUserInput(currentLogLevel))
        return false;

    // или при ошибке ввода текста сообщения,
    // или при вводе пустого сообщения пользователем,
    // завершает выполнение программы.
    if (!getMessageTextFromUserInput(messageText))
        return false;

    { // отправляет сообщение в очередь, когда воркер отдал mutex_
        std::lock_guard<std::mutex> lock(mutex_);
        logMessagesQueue_.push({messageText, currentLogLevel});
    }

    conditionVariable_.notify_one();

    return true;
}


void FileLoggerDemo::logMessagesWorker() {
    while (true) {
        LogMessage logMessage;

        { // всё в этой области выполняется "атомарно"
            std::unique_lock<std::mutex> lock(mutex_);
            conditionVariable_.wait(
                    lock, 
                    [this] {
                        return !logMessagesQueue_.empty() || !isLogging_;
                    });

            // Когда все сообщения отправлены и логирование прекращено,
            // завершает воркер.
            if (!isLogging_ && logMessagesQueue_.empty())
                break;

            // Запоминает первое в очереди сообщение в переменную
            logMessage = logMessagesQueue_.front();
            logMessagesQueue_.pop();
        }
        
        try {
            // Когда воркер отпустил mutex_,
            // можно в фоне записывать сообщение в файл.
            fileLogger_.log(logMessage.text, logMessage.logLevel);
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
