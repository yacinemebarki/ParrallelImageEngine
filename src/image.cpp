#include "../include/image.hpp"
#include "../include/ImageMatrix.hpp"
#include <string>
#include <fstream>
#include <iostream>

Image::Image(std::string path){
    this->path = path;
}

ImageMatrix Image::toMatrix(){
    std::ifstream file(this->path, std::ios::binary);

    if(!file.is_open()){
        std::cerr<<"failed to open Image";
    }

    std::cout<<"Image opened successfully";
    

}