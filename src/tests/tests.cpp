#include "../../include/FileLogger.h"
#include <filesystem>
#include <iostream>
#include <cassert>
#include <fstream>
#include <regex>


void testGetDefaultLogLevel() {
    std::cout << "\nЗАПУСК теста получения уровня важности по умолчанию\n";

    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    FileLogger fileLogger(testFile, LogLevel::WARNING);

    assert(
        fileLogger.getDefaultLogLevel() == LogLevel::WARNING
        && "ПРОВАЛ теста получения уровня важности по умолчанию\n"
    );

    std::cout << "УСПЕХ теста получения уровня важности по умолчанию\n";
}


void testSetDefaultLogLevel() {
    std::cout << "\nЗАПУСК теста получения уровня важности по умолчанию\n";

    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    FileLogger fileLogger(testFile, LogLevel::WARNING);
    fileLogger.setDefaultLogLevel(LogLevel::ERROR);

    assert(
        fileLogger.getDefaultLogLevel() == LogLevel::ERROR
        && "ПРОВАЛ теста установки уровня важности по умолчанию\n"
    );

    std::cout << "УСПЕХ теста установки уровня важности по умолчанию\n";
}


std::string readFileContent(std::string filename) {
    std::ifstream file(filename);

    std::string fileContent(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return fileContent;
}


void testLog() {
    std::cout << "\nЗАПУСК теста записи в журнал\n";

    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    {
        FileLogger fileLogger(testFile, LogLevel::INFO);
        fileLogger.log("hello world!", LogLevel::INFO);
    }

    std::string fileContent = readFileContent(testFile);
    std::filesystem::remove(testFile);

    assert(
        fileContent.find("INFO") != std::string::npos
        && "ОШИБКА: не записался уровень сообщения"
    );

    assert(
        fileContent.find("hello world!") != std::string::npos
        && "ОШИБКА: не записалось сообщение"
    );

    std::cout << "УСПЕХ теста записи в журнал\n";
}


void testLogFiltering() {
    std::cout << "\nЗАПУСК теста фильтрации\n";

    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    {
        FileLogger fileLogger(testFile, LogLevel::WARNING);
        fileLogger.log("hello world!", LogLevel::INFO);
        fileLogger.log("hello again!", LogLevel::WARNING);
        fileLogger.log("and again!", LogLevel::ERROR);
    }
    
    std::string fileContent = readFileContent(testFile);
    std::filesystem::remove(testFile);

    assert(
        fileContent.find("INFO") == std::string::npos
        && "ОШИБКА: фильтр не сработал\n"
    );

    assert(
        fileContent.find("WARNING") != std::string::npos
        && "ОШИБКА: фильтр не сработал\n"
    );

    assert(
        fileContent.find("ERROR") != std::string::npos
        && "ОШИБКА: фильтр не сработал\n"
    );

    std::cout << "УСПЕХ теста фильтрации\n";
}


void testLogLevelToString() {
    std::cout << "\nЗАПУСК теста преобразования уровня логирования в строку\n";
    LogLevel logLevel = LogLevel::INFO;
    std::string logLevelString = logLevelToString(logLevel);
    
    assert(
        logLevelString == "INFO"
        && "ПРОВАЛ теста преобразования уровня логирования в строку\n"
    );

    std::cout << "УСПЕХ теста преобразования уровня логирования в строку\n";
}


void testStringToLogLevel() {
    std::cout << "\nЗАПУСК теста преобразования строки в уровень логирования\n";

    assert(
        stringToLogLevel("INFO") == LogLevel::INFO
        && "ПРОВАЛ теста преобразования строки в уровень логирования\n"
    );

    assert(
        stringToLogLevel("WARNING") == LogLevel::WARNING
        && "ПРОВАЛ теста преобразования строки в уровень логирования\n"
    );

    assert(
        stringToLogLevel("ERROR") == LogLevel::ERROR
        && "ПРОВАЛ теста преобразования строки в уровень логирования\n"
    );
    
    std::cout << "УСПЕХ теста преобразования строки в уровень логирования\n";
}


void testExceptions() {
    std::cout << "\nЗАПУСК теста на исключения\n";

    // Проверка на пустое имя файла журнала
    bool caughtEmptyFile = false;

    try {
        FileLogger("", LogLevel::INFO);
    } catch (const std::runtime_error& e) {
        caughtEmptyFile = true;
    }

    assert(
        caughtEmptyFile
        && "ОШИБКА: не выброшено исключение для пустого имени файла журнала"
    );

    // Проверка с несуществующей строкой уровня логирования
    bool caughtInvalidLogLevelString = false;

    try {
        stringToLogLevel("UNKNOWN");
    } catch (const std::invalid_argument& e) {
        caughtInvalidLogLevelString = true;
    }

    assert(
        caughtInvalidLogLevelString
        && "ОШИБКА: не выброшено исключение для неверной строки уровня логирования"
    );

    // Проверка с неизвестным уровнем логирования
    bool caughtInvalidLogLevel = false;

    try {
        logLevelToString(static_cast<LogLevel>(666));
    } catch (const std::invalid_argument& e) {
        caughtInvalidLogLevel = true;
    }

    assert(
        caughtInvalidLogLevel
        && "ОШИБКА: не выброшено исключение для неизвестного enum"
    );

    // Проверка открытия файла в несуществующей директории
    bool caughtFileOpenError = false;

    try {
        FileLogger("abcdefghijklmnopqrstuvwxyz/test.txt", LogLevel::INFO);
    } catch (const std::exception& e) {
        caughtFileOpenError = true;
    }

    assert(
        caughtFileOpenError
        && "ОШИБКА: не выброшено исключение при ошибке открытия файла"
    );

    std::cout << "УСПЕХ теста на исключения\n";
}


void testEmptyMessage() {
    std::cout << "\nЗАПУСК теста записи пустого сообщения\n";
    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    {
        FileLogger fileLogger(testFile, LogLevel::INFO);
        fileLogger.log("", LogLevel::INFO);
    }

    std::string fileContent = readFileContent(testFile);
    std::filesystem::remove(testFile);

    assert(
        fileContent.find("[INFO]\t\n") != std::string::npos
        && "ОШИБКА: пустое сообщение записано некорректно"
    );

    std::cout << "УСПЕХ теста записи пустого сообщения\n";
}


void testLogFormat() {
    std::cout << "\nЗАПУСК теста формата вывода\n";
    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    {
        FileLogger fileLogger(testFile, LogLevel::INFO);
        fileLogger.log("format test", LogLevel::INFO);
    }

    std::string fileContent = readFileContent(testFile);
    std::filesystem::remove(testFile);

    if (!fileContent.empty() && fileContent.back() == '\n') {
        fileContent.pop_back();
    }

    // Ожидаемый формат: ЧЧ:ММ:СС.МММ [УРОВЕНЬ ЛОГИРОВАНИЯ] сообщение
    // Например: 14:05:59.123 [INFO]	format test
    std::regex logMessagePattern(R"(^\d{2}:\d{2}:\d{2}\.\d{3} \[INFO\]\tformat test$)");
    
    assert(
        std::regex_match(fileContent, logMessagePattern)
        && "ОШИБКА: формат строки лога не соответствует ожидаемому"
    );
    
    std::cout << "УСПЕХ теста формата вывода\n";
}


int main() {
    testGetDefaultLogLevel();
    testSetDefaultLogLevel();
    testLog();
    testLogFiltering();
    testLogLevelToString();
    testStringToLogLevel();
    testExceptions();
    testEmptyMessage();
    testLogFormat();

    std::cout << "\nВсе тесты успешно пройдены\n";
}
