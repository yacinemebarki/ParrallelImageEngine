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
}

ImageMetadata Image::getInfo(){
    return this->information;
}

void Image::setInfo(ImageMetadata info){
    this->information = info;
}

ImageMatrix Image::toMatrix(){
    std::vector<uint8_t> data = readFile(this->path);

    ImageType type = checkType(data);
    switch(type) {
        case ImageType::PNG:
            this->setInfo(readPngIhdr(data));
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