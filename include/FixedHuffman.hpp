#pragma once

#include "Huffman.hpp"

class FixedHuffman : public Huffman {
public:
    FixedHuffman(std::size_t litteralCount);
    void generateLiteral() override;
};
