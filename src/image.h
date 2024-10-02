#pragma once

#include <vector>

// TODO: make movable and shallow copyable
class Image
{
public:
    int width;
    int height;
    int bytesPerPixel;

    std::vector<uint8_t> pixels;
};
