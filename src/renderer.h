#pragma once

#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "vertexbuffer.h"
#include "renderable.h"
#include "vertexbufferpool.h"

class Renderer
{
public:
    Renderer(
        const std::string& vertexShaderFileName,
        const std::string& fragmentShaderFileName,
        VertexBufferPool&  vertexBufferPool);
    virtual ~Renderer();

    virtual void init();

    virtual void render(float aspectRatio, const glm::mat4& view, const std::vector<const Renderable*>& renderables) const;

protected:
    virtual void setUniforms(float aspectRatio, const Renderable& renderable) const = 0;

    const std::string vertexShaderFileName;
    const std::string fragmentShaderFileName;

    GLuint      compileShader (GLenum type, const GLchar* source);
    std::string readFile(const std::string& name) const;

    VertexBufferPool& vertexBufferPool;

    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;

    mutable glm::mat4   view;
    mutable glm::mat4   projection;

    GLuint program;
};
