CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra

LIB_NAME = liblogger.so
TARGET = main
TRASH = output.txt

all: $(TARGET)

$(LIB_NAME): Logger.cpp Logger.h
	$(CXX) $(CXXFLAGS) -fPIC -shared Logger.cpp -o $(LIB_NAME)

$(TARGET): main.cpp $(LIB_NAME)
	$(CXX) $(CXXFLAGS) main.cpp -L. -llogger -Wl,-rpath,. -o $(TARGET)

clean:
	rm -f $(TARGET) $(LIB_NAME) $(TRASH)
