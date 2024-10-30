#include "gpupool.h"

GpuPool::GpuPool() = default;
GpuPool::~GpuPool() = default;

Texture &GpuPool::get(const Image *image, Texture::Interpolation interpolation)
{
    if (!poolTextures.contains(image->getPixels()))
    {
        poolTextures.emplace(image->getPixels(), Texture());
        poolTextures[image->getPixels()].setImage(*image, interpolation);
    }
    return poolTextures[image->getPixels()];
}

VertexBuffer &GpuPool::get(const Renderable *renderable)
{

    if (!poolVertexBuffers.contains(&renderable->mesh.vertices()))
    {
        poolVertexBuffers.emplace(&renderable->mesh.vertices(), VertexBuffer());
        poolVertexBuffers[&renderable->mesh.vertices()].init();
    }
    return poolVertexBuffers[&renderable->mesh.vertices()];
}
