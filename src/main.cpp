#include <iostream>
#include <string_view>
#include "FileLogger.h"
#include "FileLoggerTester.h"
#include <cerrno>
#include <system_error>


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

    for (int i = 1; i+1 < argc; ++i) {
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
        std::cout << "Уровень сообщений по умолчанию: "
                  << logLevelToString(defaultLogLevel)
                  << "\n";

        FileLogger fileLogger(filename, defaultLogLevel);

        std::cout << "Чтобы выйти из приложения, пропустите ввод сообщения (нажмите Enter)\n";
        
        FileLoggerTester fileLoggerTester(fileLogger);
        fileLoggerTester.run();

    } catch (const std::ios_base::failure& e) {
        int systemErrno = errno; 
        
        std::cerr << "\nОшибка: не удалось открыть файл журнала\n";
        
        if (systemErrno == EACCES)
            std::cerr << "Причина: отказано в доступе к файлу журнала\n";
        else if (systemErrno == ENOENT)
            std::cerr << "Причина: указан путь к несуществующей директории\n";
        else if (systemErrno == EISDIR)
            std::cerr << "Причина: указан путь к директории, а не к файлу\n";
        
        return 1;

    } catch (const std::exception& e) {
        std::cerr << "\nПроизошла критическая ошибка: "
                  << e.what()
                  << "\n";
        return 1;
    }

    return 0;
}
