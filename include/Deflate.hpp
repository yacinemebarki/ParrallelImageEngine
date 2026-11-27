#pragma once

#include "BitReader.hpp"
#include <cstdint>
#include <vector>
#include "Huffman.hpp"

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

    explicit Deflate(BitReader& reader, Huffman& litteralTree, Huffman& distanceTree);

    Block inspectBlock();
    static void inspectZlibHeader(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decompress();

    void decompressStored(std::vector<uint8_t>& output);
    void decompressFixedHuffman(std::vector<uint8_t>& output);
    void decompressDynamicHuffman(std::vector<uint8_t>& output);

    private:
        BitReader& reader;
        Huffman& litteralTree;   
        Huffman& distanceTree;
    public: 
        uint16_t lengthBase[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
        uint8_t lengthExtra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};   

        uint16_t distanceBase[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};  
        uint8_t distanceExtra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
};
