#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ImageMatrix.hpp"

enum class ImageType {
    PNG,
    BMP,
    JPEG,
    UNKNOWN
};

std::vector<uint8_t> readFile(std::string filePath);
ImageType checkType(std::vector<uint8_t> data);
uint32_t readBigEndian32(const std::vector<uint8_t>& data, std::size_t offset);



struct ImageMetadata {
    uint32_t width;
    uint32_t height;
    uint8_t bitDepth;
    uint8_t colorType;
    uint8_t compressionMethod;
    uint8_t filterMethod;
    uint8_t interlaceMethod;
};

struct PngChunk {
    uint32_t length;
    std::string type;
    std::vector<uint8_t> data;
    uint32_t crc;
};

class Image{
    private: std::string path;  
    private: ImageMetadata information;
    private: std::vector<PngChunk> chunks;
    public: std::vector<uint8_t> data;

    public: Image(std::string path);
    
    ImageMatrix toMatrix();
    ImageMetadata getInfo();
    void setInfo(ImageMetadata info);
    std::vector<PngChunk> getChunks();
    void addChunk(PngChunk chunk);
    void praseChunk();
};