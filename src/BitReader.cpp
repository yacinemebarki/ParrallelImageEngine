#include "../include/BitReader.hpp"
#include <stdexcept>

BitReader::BitReader(const std::vector<uint8_t>& data)
    : BitReader(data, 0)
{
}

BitReader::BitReader(const std::vector<uint8_t>& data, size_t byteOffset)
    : data(data), bitPosition(byteOffset * 8)
{
    if(byteOffset > data.size()){
        throw std::out_of_range("byte offset is beyond the end of the data");
    }
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
        const size_t bitOffset = (bitPosition % 8);
        uint32_t bit = (data[byteIndex] >> bitOffset) & 1U;
        value |= (bit << i);
        ++bitPosition;
    }
    return value;
}

size_t BitReader::getPostion(){
    return this->bitPosition;
}
std::vector<uint8_t> BitReader::getData(){
    return this->data;
}