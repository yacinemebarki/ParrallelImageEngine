#include <vector>
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <iostream>

class BitReader{
    private:
    const std::vector<uint8_t>& data;
    size_t bitPosition;

    public:
    explicit BitReader(const std::vector<uint8_t>& data);
    uint32_t readBits(int n); 
};