#include "cubemap.h"
#include "errors.h"
#include "cubemaptexture.h"

#ifndef GL_TEXTURE_MAX_LEVEL
    #define GL_TEXTURE_MAX_LEVEL 0x813D
#endif

CubemapTexture::CubemapTexture()
{
}

CubemapTexture::~CubemapTexture()
{
    glDeleteTextures(1, &texture);
}


void CubemapTexture::setCubemap(const Cubemap& cubemap)
{
   glGenTextures(1, &texture);

   setImage(cubemap.positiveX, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
   setImage(cubemap.negativeX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
   setImage(cubemap.positiveY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
   setImage(cubemap.negativeY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
   setImage(cubemap.positiveZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
   setImage(cubemap.negativeZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glCheckError();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void CubemapTexture::bind(GLenum textureId)
{
    glActiveTexture(textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

int CubemapTexture::getMaxMipLevel() const
{
    return static_cast<int>(std::floor(std::log2(std::max(width, height)))) + 1;
}

void CubemapTexture::setImage(const Image &image, int side)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
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
            internalFormat  = GL_RGB;
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

    width   = image.width;
    height  = image.height;

    glTexImage2D(side,
                0,
                internalFormat,
                image.width,
                image.height,
                0,
                format,
                type,
                image.getPixels());
    glCheckError();
}
