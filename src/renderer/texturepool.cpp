#include "texturepool.h"

TexturePool::TexturePool()
{
}

TexturePool::~TexturePool()
{
}

Texture &TexturePool::get(const Image *image)
{
    if (!pool.contains(image))
    {
        pool.emplace(image, Texture());
        pool[image].setImage(*image);
    }
    return pool[image];
}
