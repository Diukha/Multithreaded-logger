CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR := src
INC_DIR := include
APP_SRCS := $(SRC_DIR)/FileLoggerTester.cpp $(SRC_DIR)/main.cpp

TESTS_SRCS := $(SRC_DIR)/tests/tests.cpp
TESTS_TARGET := tests_runner
TRASH := output.txt

LIB_NAME := liblogger.so
TARGET := main

all: $(TARGET)

# Сборка динамической библиотеки
$(LIB_NAME): $(SRC_DIR)/FileLogger.cpp $(INC_DIR)/FileLogger.h
	$(CXX) $(CXXFLAGS) -fPIC -shared $(SRC_DIR)/FileLogger.cpp -o $@

# Сборка основного исполняемого файла
$(TARGET): $(APP_SRCS) $(LIB_NAME)
	$(CXX) $(CXXFLAGS) $(APP_SRCS) -L. -llogger -Wl,-rpath,. -o $@

# Сборка исполняемого файла тестов
$(TESTS_TARGET): $(TESTS_SRCS) $(LIB_NAME)
	$(CXX) $(CXXFLAGS) $(TESTS_SRCS) -L. -llogger -Wl,-rpath,. -o $@

tests: $(TESTS_TARGET)
	./$(TESTS_TARGET)

clean:
	rm -f $(TARGET) $(LIB_NAME) $(TESTS_TARGET) $(TRASH)

.PHONY: all clean tests
