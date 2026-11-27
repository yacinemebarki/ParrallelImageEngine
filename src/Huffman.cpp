#include "../include/Huffman.hpp"
#include <algorithm>
#include <stdexcept>

Huffman::Huffman(std::size_t literalCount)
    : literalLength(literalCount), count(16, 0)
{
}

void Huffman::generateLiteralCode() {
    std::fill(count.begin(), count.end(), 0);
    literalCode.clear();

    for (uint8_t length : literalLength) {
        if (length >= count.size()) {
            throw std::invalid_argument("Huffman code length exceeds 15 bits");
        }
        ++count[length];
    }
    count[0] = 0;

    std::vector<uint16_t> nextCode(count.size(), 0);
    for (std::size_t length = 1; length < count.size(); ++length) {
        nextCode[length] = static_cast<uint16_t>(
            (nextCode[length - 1] + count[length - 1]) << 1
        );
    }

    for (std::size_t symbol = 0; symbol < literalLength.size(); ++symbol) {
        const uint8_t length = literalLength[symbol];
        if (length == 0) {
            continue;
        }

        literalCode.push_back({static_cast<uint16_t>(symbol), nextCode[length]++, length});
    }
}

const std::vector<uint8_t>& Huffman::getLiteralLength() const {
    return literalLength;
}

const std::vector<uint16_t>& Huffman::getCount() const {
    return count;
}

const std::vector<Huffman::Code>& Huffman::getLiteralCode() const {
    return literalCode;
}
