#include <iostream>
#include <fstream>
#include <iomanip>

int main() {
    std::ifstream file("image.png", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    char buffer[64];

    file.read(buffer, sizeof(buffer));

    std::cout << "Bytes read: " << file.gcount() << '\n';

    for (int i = 0; i < file.gcount(); i++) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << (static_cast<unsigned int>(
                         static_cast<unsigned char>(buffer[i])))
                  << ' ';
    }

    std::cout << '\n';

    file.close();
    return 0;
}