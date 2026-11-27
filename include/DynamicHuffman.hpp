#pragma once

#include "Huffman.hpp"

class DynamicHuffman : public Huffman {
public:
    explicit DynamicHuffman(std::vector<uint8_t> literalLengths);
    void generateLiteral() override;

private:
    std::vector<uint8_t> dynamicLiteralLength;
};
