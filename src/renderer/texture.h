#pragma once

#include <SDL_opengles2.h>
#include "image.h"

class Texture
{
public:
    Texture();
    ~Texture();

    void setImage(const Image& image);

    void bind(GLenum textureId);

private:
    GLuint       texture;

};