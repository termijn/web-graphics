#pragma once

#include "renderable.h"
#include "image.h"
#include "cubemap.h"

#include "renderer/vertexbuffer.h"
#include "renderer/texture.h"
#include "renderer/cubemaptexture.h"

#include <map>

class GpuPool
{
public:
    GpuPool();
    ~GpuPool();

    Texture&        get(const Image* image, Texture::Interpolation interpolation);
    VertexBuffer&   get(const Renderable* renderable);
    CubemapTexture& get(const Cubemap* cubemap);

private:
    std::map<const std::vector<Vertex>*, VertexBuffer>   poolVertexBuffers;
    std::map<const uint8_t*,    Texture>                 poolTextures;
    std::map<const Cubemap*, CubemapTexture>             poolCubemaps;

    GpuPool (const GpuPool&)              = delete;
    GpuPool& operator= (const GpuPool&)   = delete;

};
