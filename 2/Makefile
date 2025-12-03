CXX = g++
CXXFLAGS = -std=c++11
INCLUDES = -I/path/to/unittestpp
LIBS = -lUnitTest++

TARGET = unit_test
SOURCES = unit_test.cpp modTableCipher.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
