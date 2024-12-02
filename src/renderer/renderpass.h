#pragma once

#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "renderer/gpupool.h"
#include "renderer/rendertarget.h"

#include "renderable.h"

class RenderPass
{
public:
    RenderPass(
        const std::string&  vertexShaderFileName,
        const std::string&  fragmentShaderFileName,
        GpuPool&            gpuPool, 
        RenderTarget&       renderTarget);
    virtual ~RenderPass();

    virtual void init();

    virtual void renderPre  (const glm::mat4& view, const glm::mat4& projection);
    virtual void render     (const std::vector<const Renderable*>& renderables) const;

protected:
    virtual void setUniforms(const Renderable& renderable) const;

    const std::string vertexShaderFileName;
    const std::string fragmentShaderFileName;

    GLuint      compileShader (GLenum type, const GLchar* source);
    std::string readFile(const std::string& name) const;

    GpuPool&        gpuPool;
    RenderTarget&   renderTarget;

    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;

    mutable glm::mat4   view;
    mutable glm::mat4   projection;

    GLuint program;

    RenderPass (const RenderPass&)              = delete;
    RenderPass& operator= (const RenderPass&)   = delete;
};
