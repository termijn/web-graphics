#include "texture.h"
#include "errors.h"

Texture::Texture()
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
}

void Texture::setImage(const Image &image)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint internalFormat    = GL_RGBA;
    GLint format            = GL_RGBA;

    if (image.type == Image::Type::RG8)
    {
        internalFormat  = GL_RG8_EXT;
        format          = GL_RG_EXT;
    }

    glTexImage2D(GL_TEXTURE_2D,
                0,
                internalFormat,
                image.width,
                image.height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                image.pixels.data());
    glCheckError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}

void Texture::bind(GLenum textureId)
{
    glActiveTexture(textureId);
    glBindTexture(GL_TEXTURE_2D, texture);
}
