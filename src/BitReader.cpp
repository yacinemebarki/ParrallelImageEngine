#include "../include/BitReader.hpp"
#include <stdexcept>

BitReader::BitReader(const std::vector<uint8_t>& data)
    : data(data), bitPosition(0)
{
}

uint32_t BitReader::readBits(int n){
    if(n < 0 || n > 32){
        throw std::invalid_argument("number of bits must be between 0 and 32");
    }
    const size_t totalBits = data.size() * 8;
    if(bitPosition > totalBits || static_cast<size_t>(n) > totalBits - bitPosition){
        throw std::out_of_range("cannot read beyond the end of the data");
    }

    uint32_t value = 0;
    for(int i = 0; i < n; ++i){
        const size_t byteIndex = bitPosition / 8;
        const size_t bitOffset = 7 - (bitPosition % 8);
        value = (value << 1) | ((data[byteIndex] >> bitOffset) & 1U);
        ++bitPosition;
    }
    return value;
}