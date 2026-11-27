#include "../include/FixedHuffman.hpp"

FixedHuffman::FixedHuffman()
    : Huffman(288)
{
}

void FixedHuffman::generateLiteral() {
    for (std::size_t symbol = 0; symbol <= 143; ++symbol) {
        literalLength[symbol] = 8;
    }
    for (std::size_t symbol = 144; symbol <= 255; ++symbol) {
        literalLength[symbol] = 9;
    }
    for (std::size_t symbol = 256; symbol <= 279; ++symbol) {
        literalLength[symbol] = 7;
    }
    for (std::size_t symbol = 280; symbol <= 287; ++symbol) {
        literalLength[symbol] = 8;
    }
}
