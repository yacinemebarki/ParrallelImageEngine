#include <iostream>
#include <fstream>
#include <iomanip>
#include "../include/image.hpp"
#include <vector>

int main() {
    std::vector<uint8_t> data = readFile("main/image.png");
    ImageType type = checkType(data);
    std::vector<uint8_t> data_test = {0x00, 0x00, 0x02, 0x38};
    std::cout << "The result is = "<< std::dec << readBigEndian32(data_test, 0) << '\n';
    return 0;
}