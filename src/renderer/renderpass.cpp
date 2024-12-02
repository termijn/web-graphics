#include "renderer/renderpass.h"
#include "errors.h"

#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderpass.h"

using namespace glm;

RenderPass::RenderPass(const std::string &vertexShaderFileName_, const std::string &fragmentShaderFileName_, GpuPool& gpuPool_, RenderTarget& renderTarget_)
    : gpuPool               (gpuPool_)
    , renderTarget          (renderTarget_)
    , vertexShaderFileName  (vertexShaderFileName_)
    , fragmentShaderFileName(fragmentShaderFileName_)
{
}

RenderPass::~RenderPass() = default;

void RenderPass::init()
{
    fragmentShader  = gpuPool.compileShader(GL_VERTEX_SHADER, vertexShaderFileName);
    vertexShader    = gpuPool.compileShader(GL_FRAGMENT_SHADER, fragmentShaderFileName);

    program = glCreateProgram();
    glAttachShader  (program, vertexShader);
    glAttachShader  (program, fragmentShader);
    glLinkProgram   (program);
}

void RenderPass::renderPre(const glm::mat4 &view_, const glm::mat4 &projection_)
{
    view        = view_;
    projection  = projection_;

    glUseProgram(program);

    renderTarget.beginRender();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void RenderPass::render(const std::vector<const Renderable *> &renderables) const
{
    for (const Renderable* renderable : renderables)
    {
        VertexBuffer& vertexBuffer = gpuPool.get(renderable);
        vertexBuffer.bind(program);

        setUniforms(*renderable);

        glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices().size() * 3, GL_UNSIGNED_INT, 0);
        glCheckError();
    }

    renderTarget.endRender();
}

void RenderPass::setUniforms(const Renderable &renderable) const
{
}
