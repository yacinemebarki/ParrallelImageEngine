#include <string>
#include <vector>

class Image{
    private: std::string path;  

    Image(std::string path);
    
    std::vector<std::vector<float>> toMatrix();
};