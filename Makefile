CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

TARGET = imageengine
TEST_TARGET = test_runner

MAIN = main/main.cpp
IMAGE = src/image.cpp
BITREADER = src/BitReader.cpp
PIXEL = src/pixel.cpp
IMAGEMATRIX = src/ImageMatrix.cpp
READPNG_TEST = test/test_readPng.cpp

SOURCES = $(MAIN) $(IMAGE) $(BITREADER) $(PIXEL) $(IMAGEMATRIX)
TEST_SOURCES = $(READPNG_TEST) $(IMAGE) $(PIXEL) $(IMAGEMATRIX)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

$(TEST_TARGET): $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -lgtest -lgtest_main -pthread -o $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)

.PHONY: run test clean
