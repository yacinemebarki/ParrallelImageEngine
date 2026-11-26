#include "../include/Deflate.hpp"
#include <stdexcept>

Deflate::Deflate(BitReader& reader)
    : reader(reader), literalLength(288), literalDistance(32, 5), count(16, 0)
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

void Deflate::generateLiteral() {
    for(int i = 0; i <= 143; i++) {
        literalLength[i] = 8;
    }

    for(int i = 144; i <= 255; i++) {
        literalLength[i] = 9;
    }

    for(int i = 256; i <= 279; i++) {
        literalLength[i] = 7;
    }

    for(int i = 280; i <= 287; i++) {
        literalLength[i] = 8;
    }

}

void Deflate::generateLiteralCode() {
    for(uint8_t len : literalLength) {
        count[len]++;
    }    
    count[0] = 0;

    std::vector<uint16_t> nextCode(16, 0);

    for(int len = 1; len < 16; len++) {
        nextCode[len] = (nextCode[len - 1] + count[len - 1]) << 1;
    }
}

std::vector<uint8_t> Deflate::decompressStored(){
    std::vector<uint8_t> output;
    reader.alignToByte();

    uint16_t len = reader.readBits(16);
    uint16_t nlen = reader.readBits(16);

    if ((len ^ nlen) != 0xFFFF) {
        throw std::runtime_error("Invalid stored block");
    }

    for(size_t i = 0; i < len; i++) {
        uint8_t byte = static_cast<uint8_t>(reader.readBits(8));
        output.push_back(byte);        
    }
    
    return output;
}

std::vector<uint8_t> Deflate::decompressFixedHuffman() {

}

std::vector<uint8_t> Deflate::decompressDynamicHuffman() {

}

std::vector<uint8_t> Deflate::decompress() {
    std::vector<uint8_t> output;
    std::vector<uint8_t> blockData;


    while(true) {
        Block block = inspectBlock();

        switch(block.type) {

            case BlockType::Stored:
                blockData = decompressStored();
                break;

            case BlockType::FixedHuffman:
                blockData = decompressFixedHuffman();
                break;

            case BlockType::DynamicHuffman:
                blockData = decompressDynamicHuffman();
                break;
            
        }
        output.insert(output.end(), blockData.begin(), blockData.end());

        if(block.isFinal) {
            break;
        }
    }

    return output;
}
