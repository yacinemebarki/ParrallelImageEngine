#include "../include/image.hpp"
#include "../include/ImageMatrix.hpp"
#include <stdexcept>

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

ImageMatrix Image::toMatrix(){
    ImageType type = checkType(this->data);
    std::vector<uint8_t> compressedData;
    switch(type) {
        case ImageType::PNG:
            this->praseChunk();
            this->setInfo(readPngIhdr(this->chunks[0]));
            compressedData = this->extractIdat();
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