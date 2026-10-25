#include <string>
#include <vector>
#include "ImageMatrix.hpp"

class Image{
    private: std::string path;  

    public: Image(std::string path);
    
    ImageMatrix toMatrix();
};