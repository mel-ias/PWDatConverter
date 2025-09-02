CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17

SRC = PWConverter.cpp PWDatConverter.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = PWDatConverter

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)