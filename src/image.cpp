#include "../include/image.hpp"
#include "../include/ImageMatrix.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>


Image::Image(std::string path){
    this->path = path;
}

ImageMatrix Image::toMatrix(){
    std::ifstream file(this->path, std::ios::binary);

    if(!file.is_open()){
        std::cerr<<"failed to open Image";
    }

    file.seekg(0, std::ios::end);
    
    std::streamsize fileSize = file.tellg();

    file.seekg(0, std::ios::beg);

    std::vector<uint8_t>  buffer(fileSize);

    if(!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)){
        std::cerr<<"failed to read the image";
    }

    std::cout<<"Image opened successfully";


}