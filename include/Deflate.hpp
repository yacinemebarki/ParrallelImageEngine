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

    struct HuffmanCode {
        uint16_t symbol;
        uint16_t code;
        uint8_t length;
    };

    explicit Deflate(BitReader& reader);

    Block inspectBlock();
    static void inspectZlibHeader(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompress();

    std::vector<uint8_t> decompressStored();
    std::vector<uint8_t> decompressFixedHuffman();
    std::vector<uint8_t> decompressDynamicHuffman();
    void generateLiteral();
    void generateLiteralCode();

    private:
        BitReader& reader;
    
    public:
        std::vector<uint8_t> literalLength;
        std::vector<size_t> literalDistance;
        std::vector<HuffmanCode> literalCode;
        std::vector<uint16_t> count;
};
