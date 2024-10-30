#pragma once

#include "renderable.h"
#include "image.h"

#include "renderer/vertexbuffer.h"
#include "renderer/texture.h"

#include <map>

class GpuPool
{
public:
    GpuPool();
    ~GpuPool();

    Texture&        get(const Image* image, Texture::Interpolation interpolation);
    VertexBuffer&   get(const Renderable* renderable);

private:
    std::map<const std::vector<Vertex>*, VertexBuffer>   poolVertexBuffers;
    std::map<const uint8_t*,    Texture>                 poolTextures;

    GpuPool (const GpuPool&)              = delete;
    GpuPool& operator= (const GpuPool&)   = delete;

};