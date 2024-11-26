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
        poolVertexBuffers[&renderable->mesh.vertices()].setMesh(&renderable->mesh);
    }
    return poolVertexBuffers[&renderable->mesh.vertices()];
}

CubemapTexture &GpuPool::get(const Cubemap *cubemap)
{
    if (!poolCubemaps.contains(cubemap))
    {
        poolCubemaps.emplace(std::piecewise_construct,
                          std::forward_as_tuple(cubemap),
                          std::tuple<>());
        poolCubemaps[cubemap].setCubemap(*cubemap);
    }
    return poolCubemaps[cubemap];
}
