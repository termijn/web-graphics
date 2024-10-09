#pragma once

#include <vector>

// TODO: make movable and shallow copyable, support multiple types
class Image
{
public:
    enum class Type {
        RGBA,
        RG8
    };

    int width;
    int height;
    int bytesPerPixel = 4;
    Type type = Type::RGBA;

    void setPixel(int x, int y, uint8_t value);
    void setPixel(int x, int y, uint8_t valueR, uint8_t valueG);
    void setPixel(int x, int y, uint16_t value);
    void setPixel(int x, int y, uint32_t value);

    int makePoissonDisc(int width, int height, int minDistance);
    int sizeInBytes() const;

    std::vector<uint8_t> pixels;
};
