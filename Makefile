CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR := src
INC_DIR := include

LIB_NAME := liblogger.so
TARGET := main
TRASH := output.txt

# Исходные файлы самого тестового приложения
APP_SRCS := $(SRC_DIR)/FileLoggerTester.cpp $(SRC_DIR)/main.cpp

all: $(TARGET)

# Сборка динамической библиотеки
$(LIB_NAME): $(SRC_DIR)/FileLogger.cpp $(INC_DIR)/FileLogger.h
	$(CXX) $(CXXFLAGS) -fPIC -shared $(SRC_DIR)/FileLogger.cpp -o $@

# Сборка исполняемого файла
$(TARGET): $(APP_SRCS) $(LIB_NAME)
	$(CXX) $(CXXFLAGS) $(APP_SRCS) -L. -llogger -Wl,-rpath,. -o $@

clean:
	rm -f $(TARGET) $(LIB_NAME) $(TRASH)

.PHONY: all clean
