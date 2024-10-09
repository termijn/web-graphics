#include "gpupool.h"

GpuPool::GpuPool() = default;
GpuPool::~GpuPool() = default;

Texture &GpuPool::get(const Image *image)
{
    if (!poolTextures.contains(image))
    {
        poolTextures.emplace(image, Texture());
        poolTextures[image].setImage(*image);
    }
    return poolTextures[image];
}

VertexBuffer &GpuPool::get(const Renderable *renderable)
{
    if (!poolVertexBuffers.contains(renderable))
    {
        poolVertexBuffers.emplace(renderable, VertexBuffer());
        poolVertexBuffers[renderable].init();
    }
    return poolVertexBuffers[renderable];
}
