#include <iostream>
#include "Logger.h"

int main() {
    FileLogger logger = FileLogger("output.txt", LogLevel::INFO);
    logger.setDefaultLogLevel(LogLevel::WARNING);
    
    std::cout << logLevelToString(logger.getDefaultLogLevel()) << std::endl;
    logger.log("12345", LogLevel::WARNING);

    return 0;
}
