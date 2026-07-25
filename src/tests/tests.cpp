#include "../../include/FileLogger.h"
#include <filesystem>
#include <iostream>
#include <cassert>
#include <fstream>


void testGetDefaultLogLevel() {
    std::cout << "\nЗАПУСК теста получения уровня важности по умолчанию\n";

    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    FileLogger fileLogger(testFile, LogLevel::WARNING);

    if (fileLogger.getDefaultLogLevel() == LogLevel::WARNING)
        std::cout << "УСПЕХ теста получения уровня важности по умолчанию\n";
    else
        std::cerr << "ПРОВАЛ теста получения уровня важности по умолчанию\n";

    assert(fileLogger.getDefaultLogLevel() == LogLevel::WARNING);
}


void testSetDefaultLogLevel() {
    std::cout << "\nЗАПУСК теста получения уровня важности по умолчанию\n";

    std::string testFile = "test_output.txt";
    std::filesystem::remove(testFile);

    FileLogger fileLogger(testFile, LogLevel::WARNING);
    fileLogger.setDefaultLogLevel(LogLevel::ERROR);

    if (fileLogger.getDefaultLogLevel() == LogLevel::ERROR)
        std::cout << "УСПЕХ теста установки уровня важности по умолчанию\n";
    else
        std::cerr << "ПРОВАЛ теста установки уровня важности по умолчанию\n";

    assert(fileLogger.getDefaultLogLevel() == LogLevel::ERROR);
}


std::string readFileContent(std::string filename) {
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
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
    
    if (fileContent.find("INFO") != std::string::npos 
        && fileContent.find("hello world!") != std::string::npos)
        std::cout << "УСПЕХ теста записи в журнал\n";
    else
        std::cerr << "ПРОВАЛ теста записи в журнал\n";

    assert(fileContent.find("INFO") != std::string::npos && "ОШИБКА: не записался уровень сообщения");
    assert(fileContent.find("hello world!") != std::string::npos && "ОШИБКА: не записалось сообщение");
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

    if (fileContent.find("hello world!") == std::string::npos
        && fileContent.find("hello again!") != std::string::npos
        && fileContent.find("and again!") != std::string::npos)
        std::cout << "УСПЕХ теста фильтрации\n";
    else
        std::cerr << "ПРОВАЛ теста фильтрации\n";

    assert(fileContent.find("INFO") == std::string::npos && "ОШИБКА: фильтр не сработал");
}

int main() {
    testGetDefaultLogLevel();
    testSetDefaultLogLevel();
    testLog();
    testLogFiltering();
}
