#pragma once

#include <SDL_opengles2.h>
#include "image.h"

class Texture
{    
public:
    enum class Interpolation {
        Linear,
        Nearest
    };

    Texture();
    ~Texture();

    void setImage(const Image& image, Interpolation interpolation);

    void bind(GLenum textureId);

private:
    GLuint       texture;

};