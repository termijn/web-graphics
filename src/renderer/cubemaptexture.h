#pragma once

#include <SDL_opengles2.h>
#include "cubemap.h"

class CubemapTexture
{    
public:
    CubemapTexture();
    ~CubemapTexture();

    void setCubemap(const Cubemap& cubemap);

    void bind(GLenum textureId);

private:
    GLuint       texture;

    void setImage(const Image& image, int side);

    CubemapTexture (const CubemapTexture&)              = delete;
    CubemapTexture& operator= (const CubemapTexture&)   = delete;

};