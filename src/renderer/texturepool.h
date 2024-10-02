#pragma once

#include "renderer/texture.h"
#include "image.h"
#include <map>

class TexturePool
{
public:
    TexturePool();
    ~TexturePool();

    Texture& get(const Image* image);

private:
    std::map<const Image*, Texture> pool;

    TexturePool (const TexturePool&)              = delete;
    TexturePool& operator= (const TexturePool&)   = delete;

};