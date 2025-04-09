CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = parser

SRCS = parser.cpp
HEADERS = Graph.hpp

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
