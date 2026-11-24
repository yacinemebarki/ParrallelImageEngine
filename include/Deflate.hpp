#pragma once

#include "BitReader.hpp"
#include <cstdint>
#include <vector>

class Deflate {
public:
    enum class BlockType {
        Stored = 0,
        FixedHuffman = 1,
        DynamicHuffman = 2
    };

    struct Block {
        bool isFinal;
        BlockType type;
    };

    struct StoredBlock {
        uint16_t length;
        uint16_t inverseLength;
        std::vector<uint8_t> data;
    };

    explicit Deflate(BitReader& reader);

    Block inspectBlock();
    static void inspectZlibHeader(const std::vector<uint8_t>& data);
    

    private:
        BitReader& reader;
};
