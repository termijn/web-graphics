#include "texture.h"
#include "errors.h"

#ifndef GL_HALF_FLOAT
    #define GL_HALF_FLOAT 0x140B
#endif
#ifndef GL_RGBA16F
    #define GL_RGBA16F 0x881A
#endif

Texture::Texture()
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &texture);
}

void Texture::setImage(const Image &image, Interpolation interpolation)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint internalFormat    = GL_RGBA;
    GLint format            = GL_RGBA;
    GLint type              = GL_UNSIGNED_BYTE;

    switch (image.type)
    {
        case Image::Type::RG8:
            format          = GL_RG_EXT;
            internalFormat  = GL_RG8_EXT;
            type            = GL_UNSIGNED_BYTE;
            break;
        case Image::Type::R8:
            format          = GL_R8_EXT;
            internalFormat  = GL_R8_EXT;
            type            = GL_UNSIGNED_BYTE;
            break;
        case Image::Type::RGB:
            format          = GL_RGB;
            internalFormat  = GL_RGBA;
            type            = GL_UNSIGNED_BYTE;
            break;
        case Image::Type::RGBA:
            format          = GL_RGBA;
            internalFormat  = GL_RGBA;
            type            = GL_UNSIGNED_BYTE;
            break;
        case Image::Type::RGBA16:
            internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT;
            break;
    }

    glTexImage2D(GL_TEXTURE_2D,
                0,
                internalFormat,
                image.width,
                image.height,
                0,
                format,
                type,
                image.pixels.data());
    glCheckError();

    switch (interpolation)
    {
    case Interpolation::Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case Interpolation::Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::bind(GLenum textureId)
{
    glActiveTexture(textureId);
    glBindTexture(GL_TEXTURE_2D, texture);
}
