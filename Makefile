CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = imageengine

MAIN = main/main.cpp
IMAGE = src/image.cpp
BITREADER = src/BitReader.cpp
PIXEL = src/pixel.cpp
IMAGEMATRIX = src/ImageMatrix.cpp

SOURCES = $(MAIN) $(IMAGE) $(BITREADER) $(PIXEL) $(IMAGEMATRIX)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
