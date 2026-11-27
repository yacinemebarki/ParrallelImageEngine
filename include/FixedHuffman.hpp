#pragma once

#include "Huffman.hpp"

class FixedHuffman : public Huffman {
public:
    FixedHuffman();
    void generateLiteral() override;
};
