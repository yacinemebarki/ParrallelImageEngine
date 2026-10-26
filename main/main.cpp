#include <iostream>
#include <fstream>
#include <iomanip>
#include "../include/image.hpp"
#include <vector>

int main() {
    std::vector<uint8_t> data = readFile("main/image.png");
    return 0;
}