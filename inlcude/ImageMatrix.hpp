#include <vector>
#include "pixel.hpp"

class ImageMatrix{
    std::vector<std::vector<Pixel>> matrix;

    public: ImageMatrix(std::vector<std::vector<Pixel>> matrix);

    void setPixel(int row, int column, Pixel pixel);
    Pixel getPixel(int row, int column);

};