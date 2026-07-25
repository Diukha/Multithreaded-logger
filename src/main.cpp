#include <iostream>
#include <string_view>
#include "FileLogger.h"
#include "FileLoggerTester.h"

std::string filename = "";
LogLevel defaultLogLevel = LogLevel::INFO;

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

int main(int argc, char* argv[]) {
    try {
        if (!parseArgs(argc, argv)) return 1;
        
        std::cout << "Файл журнала: " << filename << "\n";
        std::cout << "Уровень сообщений по умолчанию: " << logLevelToString(defaultLogLevel) << "\n";

        FileLogger fileLogger(filename, defaultLogLevel);
        
        FileLoggerTester tester(fileLogger);
        tester.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Произошла критическая ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
