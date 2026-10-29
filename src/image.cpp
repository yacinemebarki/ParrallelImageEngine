#include "../include/image.hpp"
#include "../include/ImageMatrix.hpp"
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

uint32_t readBigEndian32(const std::vector<uint8_t>& data, size_t offset){
    uint32_t result = 0;
    for(int i = 0; i < 4; i++){
        uint32_t value = data[offset + i];
        result |= value << (8 * (3 - i));
    }
    return result;
}

ImageMetadata readPngIhdr(std::vector<uint8_t>& data){
    if(data.size() < 29){
        throw std::runtime_error("invalid png format");
    }
    ImageMetadata info;
    info.bitDepth = data[24];
    info.colorType = data[25];
    info.compressionMethod = data[26];
    info.filterMethod = data[27];
    info.interlaceMethod = data[28]; 
    info.width = readBigEndian32(data, 16);
    info.height = readBigEndian32(data, 20);
    return info; 
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
        
        if(notfit){
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

        this->addChunk(chunk);

        if(chunk.type == "IEND"){
            if(chunk.length != 0){
                throw std::runtime_error("Invalid IEND chunk");
            }
            foundIEND = true;
            break;
        }
    }
    if (!foundIEND) {
        throw std::runtime_error("PNG is missing IEND");
    }
}

ImageMatrix Image::toMatrix(){
    ImageType type = checkType(this->data);
    switch(type) {
        case ImageType::PNG:
            this->praseChunk();
            this->setInfo(readPngIhdr(this->data));
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