#include "../include/Deflate.hpp"
#include <stdexcept>

Deflate::Deflate(BitReader& reader, Huffman& litteralTree, Huffman& distanceTree)
    : reader(reader), litteralTree(litteralTree), distanceTree(distanceTree)
{
}

Deflate::Block Deflate::inspectBlock() {
    Block block;
    block.isFinal = reader.readBits(1) != 0;

    const uint8_t blockType = static_cast<uint8_t>(reader.readBits(2));
    if (blockType == 3) {
        throw std::runtime_error("Reserved DEFLATE block type");
    }

    block.type = static_cast<BlockType>(blockType);
    return block;
}

void Deflate::inspectZlibHeader(const std::vector<uint8_t>& data) {
    if (data.size() < 6) {
        throw std::runtime_error("invalid zlib size");
    }

    const uint8_t cmf = data[0];
    const uint8_t flg = data[1];

    if ((cmf >> 4) > 7) {
        throw std::runtime_error("invalid zlib window size");
    }

    const uint8_t compressionMethod = cmf & 0x0F;
    if (compressionMethod != 8) {
        throw std::runtime_error("not deflate comparision method");
    }

    const uint16_t header = (static_cast<uint16_t>(cmf) << 8) | flg;
    if (header % 31 != 0) {
        throw std::runtime_error("invalid zlib checksum");
    }

    const bool hasDictionary = (flg & 0x20) != 0;
    if (hasDictionary) {
        throw std::runtime_error("PNG does not support preset dictionaries");
    }
}

void Deflate::decompressStored(std::vector<uint8_t>& output){
    reader.alignToByte();

    uint16_t len = reader.readBits(16);
    uint16_t nlen = reader.readBits(16);

    if ((len ^ nlen) != 0xFFFF) {
        throw std::runtime_error("Invalid stored block");
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = static_cast<uint8_t>(reader.readBits(8));
        output.push_back(byte);
    }
}

void Deflate::decompressFixedHuffman(std::vector<uint8_t>& output) {   
    uint16_t symbol;
    while(true) {
        symbol = litteralTree.decodeSymbol(reader);
        if(symbol == 256) {
            break;
        }
        if(symbol <= 255) {
            output.push_back(static_cast<uint8_t>(symbol));
        }
        else if(symbol <= 285) {
            int index = symbol - 257;
            
            int len = lengthBase[index];
            int extrat = lengthExtra[index];
            len += static_cast<int>(reader.readBits(extrat));
            
            int distanceSymbol = distanceTree.decodeSymbol(reader);
            
            int distance = distanceBase[distanceSymbol];
            int extraDistance = distanceExtra[distanceSymbol];

            distance += static_cast<int>(reader.readBits(extraDistance));

            if(distance >  static_cast<int>(output.size())) {
                throw std::runtime_error("Invalid distance");
            }

            for(int i = 0; i < len; i++) {
                output.push_back(output[output.size() - distance]);
            }
        }
        else {
            throw std::runtime_error("Invalid literal/length symbol");
        }
    }
}

void Deflate::decompressDynamicHuffman(std::vector<uint8_t>& output) {
    
}

std::vector<uint8_t> Deflate::decompress() {
    std::vector<uint8_t> output;

    while(true) {
        Block block = inspectBlock();

        switch(block.type) {

            case BlockType::Stored:
                decompressStored(output);
                break;

            case BlockType::FixedHuffman:
                decompressFixedHuffman(output);
                break;

            case BlockType::DynamicHuffman:
                decompressDynamicHuffman(output);
                break;
            
        }

        if(block.isFinal) {
            break;
        }
    }

    return output;
}
