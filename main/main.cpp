#include <iostream>
#include <iomanip>
#include "image.hpp"

int main() {
    Image image("main/image.png");

    image.toMatrix();

    std::vector<uint8_t> compressedData = image.extractIdat();

    std::cout << "IDAT extracted successfully!\n";
    std::cout << "Total compressed bytes: " << compressedData.size() << "\n";

    std::cout << "First 20 bytes:\n";

    for (size_t i = 0; i < std::min(compressedData.size(), size_t(20)); i++) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << static_cast<unsigned int>(compressedData[i])
                  << " ";
    }

    std::cout << std::dec << "\n";

    return 0;
}