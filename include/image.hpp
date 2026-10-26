#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ImageMatrix.hpp"

std::vector<uint8_t> readFile(std::string filePath);

class Image{
    private: std::string path;  

    public: Image(std::string path);
    
    ImageMatrix toMatrix();
};