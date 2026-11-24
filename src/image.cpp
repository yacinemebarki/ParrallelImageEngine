#include "../include/image.hpp"
#include "../include/ImageMatrix.hpp"
#include "../include/BitReader.hpp"
#include <iomanip>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
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



Image::Image(std::string path){
    this->path = path;
    this->data = readFile(path);
}

ImageMetadata Image::getInfo(){
    return this->information;
}

void Image::setInfo(ImageMetadata info){
    this->information = info;
}
void Image::addChunk(PngChunk chunk){
    this->chunks.push_back(chunk);
}
std::vector<PngChunk> Image::getChunks(){
    return this->chunks;
}
void Image::praseChunk(){
    this->chunks.clear();

    size_t offset = 8;
    bool foundIEND = false;

    while(offset < this->data.size()){
        if(this->data.size() - offset < 12){
            throw std::runtime_error("each chunk must have 12 bytes");
        }

        PngChunk chunk;
        chunk.length = readBigEndian32(this->data, offset);
        offset += 4;

        chunk.type = std::string(reinterpret_cast<const char*>(this->data.data() + offset ), 4);
        offset += 4;

        bool notfit = chunk.length > this->data.size() - offset - 4;
        
        if(notfit || this->data.size() - offset < 4){
            throw std::runtime_error("invalid PNG chunk length");
        }

        chunk.data.assign(this->data.begin() + offset, this->data.begin() + chunk.length + offset);
        offset += chunk.length;

        chunk.crc = readBigEndian32(data, offset);
        offset += 4;

        bool notIHDR = chunk.type != "IHDR" || chunk.length != 13;
        if(this->chunks.empty() && notIHDR){
            throw std::runtime_error("the first chunk must be ihdr");
        }

        if(!this->chunks.empty() && !notIHDR){
            throw std::runtime_error("found ihdr in the midle");
        }

        this->addChunk(chunk);

        if(chunk.type == "IEND"){
            if(chunk.length != 0){
                throw std::runtime_error("Invalid IEND chunk");
            }
            foundIEND = true;
            break;
        }
    }
    if (offset != this->data.size()) {
        throw std::runtime_error("Unexpected data after IEND");
    }
    if (!foundIEND) {
        throw std::runtime_error("PNG is missing IEND");
    }
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

std::vector<uint8_t> Image::extractIdat(){
    std::vector<uint8_t> compressedData;
    for(const PngChunk& chunk : this->chunks) {
        if(chunk.type == "IDAT") {
            compressedData.insert(compressedData.end(), chunk.data.begin(), chunk.data.end());
        }
    }
    if (compressedData.empty()) {
        throw std::runtime_error("PNG contains no IDAT data");
    }
    return compressedData;
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

ImageMatrix Image::toMatrix(){
    ImageType type = checkType(this->data);
    switch(type) {
        case ImageType::PNG:
            this->praseChunk();
            this->setInfo(readPngIhdr(this->chunks[0]));
            break;
        case ImageType::JPEG:
            break;
        case ImageType::BMP:
            break;
        default:
            throw std::runtime_error("Unsupported image format");
    }

    return ImageMatrix(std::vector<std::vector<Pixel>>{});
}