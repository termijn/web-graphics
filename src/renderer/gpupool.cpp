#include "gpupool.h"
#include <iostream>
#include <fstream>

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

GLuint GpuPool::compileShader(GLenum type, const std::string& filename) const
{
    std::string source = readFile(filename);
    return compileShaderInternal(type, source.c_str());
}

std::string GpuPool::readFile(const std::string &name) const
{
    std::string result;
    std::ifstream stream(name);
    if (!stream.is_open()) 
    {
        std::cerr << "Error: Could not open " << name  << std::endl;
        return result;
    }
    std::string line;

    while (std::getline(stream, line)) 
    {
        result += line + "\n";
    }
    stream.close();
    return result;
}

GLuint GpuPool::compileShaderInternal(GLenum type, const GLchar *source) const
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) 
    {
        std::string message;
        message.resize(infoLen);

        glGetShaderInfoLog(shader, infoLen, NULL, message.data());
        std::cout << message << "\n";
    } 
    else 
    {
        std::cout << "shader compiled\n";
    }

    return shader;
}
