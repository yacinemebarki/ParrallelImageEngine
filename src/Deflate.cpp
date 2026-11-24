#include "../include/Deflate.hpp"
#include <stdexcept>

Deflate::Deflate(BitReader& reader)
    : reader(reader)
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
