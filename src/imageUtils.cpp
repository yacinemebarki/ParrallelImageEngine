#include "../include/image.hpp"
#include "../include/BitReader.hpp"
#include <iomanip>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::vector<uint8_t> readFile(std::string filePath){
    std::ifstream file(filePath, std::ios::binary);

    if(!file.is_open()){
        throw std::runtime_error("Failed to open image");
    }

    file.seekg(0, std::ios::end);
    
    std::streamsize fileSize = file.tellg();

    file.seekg(0, std::ios::beg);

    std::vector<uint8_t>  buffer(fileSize);

    if(!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)){
        throw std::runtime_error("Failed to read image");
    }
    std::cout<<"file was read successfully\n";
    for (int i = 0; i < 14; i++) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0')
                  << (static_cast<unsigned int>(
                         static_cast<unsigned char>(buffer[i])))
                  << ' ';
    }
    return buffer;
}

//for now it accept only png image
ImageType checkType(std::vector<uint8_t> data){   
    const uint8_t PngSignature[8] = {0x89, 0x50, 0x4E, 0x47,0x0D, 0x0A, 0x1A, 0x0A};
    if(data.size() < 8){
        throw std::runtime_error("the image is small to be png");
    }

    for(int i = 0; i < 8; i++){
        if(data[i] != PngSignature[i]){
            throw std::runtime_error("invalid png format");
        }
    }
    std::cout<<"Valid png signature";
    return ImageType::PNG;
}

uint32_t readBigEndian32(const std::vector<uint8_t>& data, size_t offset) {
    if (offset > data.size() || data.size() - offset < 4) {
        throw std::out_of_range("Not enough data");
    }

    return (static_cast<uint32_t>(data[offset]) << 24) |
           (static_cast<uint32_t>(data[offset + 1]) << 16) |
           (static_cast<uint32_t>(data[offset + 2]) << 8) |
           static_cast<uint32_t>(data[offset + 3]);
}

ImageMetadata readPngIhdr(PngChunk& chunk) {
    if (chunk.type != "IHDR" || chunk.length != 13) {
        throw std::runtime_error("Invalid IHDR chunk");
    }

    ImageMetadata info;

    info.width = readBigEndian32(chunk.data, 0);
    info.height = readBigEndian32(chunk.data, 4);

    info.bitDepth = chunk.data[8];
    info.colorType = chunk.data[9];
    info.compressionMethod = chunk.data[10];
    info.filterMethod = chunk.data[11];
    info.interlaceMethod = chunk.data[12];

    return info;
}

void inspectZlibHeader(const std::vector<uint8_t>& data) {
    if(data.size() < 6) {
        throw std::runtime_error("invalid zlib size");
    }

    uint8_t cmf = data[0];
    uint8_t flg = data[1];

    if ((cmf >> 4) > 7) {
        throw std::runtime_error("invalid zlib window size");
    }

    uint8_t comparisonMethod = cmf & 0x0F;

    if(comparisonMethod != 8) {
        throw std::runtime_error("not deflate comparision method");
    }

    uint16_t header = (static_cast<uint16_t>(cmf) << 8) | flg;

    if(header % 31 != 0) {
        throw std::runtime_error("invalid zlib checksum");
    }

    bool hasDictionary = (flg & 0x20) != 0;

    if (hasDictionary) {
        throw std::runtime_error("PNG does not support preset dictionaries");
    }
}

DeflateBlock inspectDeflateBlock(BitReader& reader) {
    DeflateBlock block;

    bool bfinal = reader.readBits(1) != 0;
    uint8_t btype = reader.readBits(2) & 0x03;

    block.isFinal = bfinal;

    if (btype == 3) {
        throw std::runtime_error("Reserved DEFLATE block type");
    }

    block.type = static_cast<DeflateBlockType>(btype);
    return block;
}
