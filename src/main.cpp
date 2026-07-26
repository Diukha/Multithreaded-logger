#include <iostream>
#include <string_view>
#include "FileLoggerDemo.h"
#include <cerrno>


// Для инициализации библиотеки.
std::string filename = "";
LogLevel defaultLogLevel = LogLevel::INFO;


void printHelp() {
    std::cout
        << "Это приложение для проверки библиотеки записи сообщений в журнал.\n"
        << "Использование: app [ОПЦИЯ] [ЗНАЧЕНИЕ]\n"
        << "Пример использования: app -o output.txt -l ERROR\n"
        << "Доступные опции:\n"
        << "  -o, --output PATH/TO/FILE     используется для указания файла журнала\n"
        << "  -l, --log-level LEVEL         используется для указания уровня важности сообщения по умолчанию\n"
        << "  -h, --help                    показать эту подсказку\n";
}


bool parseArgs(int argc, char* argv[]) {
    if (argc == 1) { // приложение запущено без аргументов
        printHelp();
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printHelp();
            return false;
        }

        if (arg == "-o" || arg == "--output") {
            // следующим аргументом должно быть имя файла
            if (i + 1 < argc) {
                filename = argv[++i]; 
            } else {
                std::cerr << "Ошибка: после флага -o не указано имя файла.\n";
                return false;
            }
        }

        if (arg == "-l" || arg == "--log-level") {
            // следующим аргументом должен быть уровень важности
            if (i + 1 < argc) {
                defaultLogLevel = stringToLogLevel(argv[++i]);
            } else {
                std::cerr << "Ошибка: после флага -l не указан уровень важности.\n";
                return false;
            }
        }
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
        
        FileLoggerDemo fileLoggerDemo(fileLogger);

        // Запуск демонстрации работы библиотеки с пользовательским вводом.
        fileLoggerDemo.run();

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
