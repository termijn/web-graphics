#include "renderer/shadowpass.h"
#include "iostream"
#include "errors.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "shadowpass.h"

using namespace glm;

ShadowPass::ShadowPass(GpuPool& gpuPool_)
    : RenderPass("/package/shaders/shadowmap-vertex.glsl", "/package/shaders/shadowmap-frag.glsl", gpuPool_)
{

}

ShadowPass::~ShadowPass() = default;

void ShadowPass::init()
{
    RenderPass::init();

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers     (1, &framebuffer);
    glBindFramebuffer     (GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           depthTexture,
                            0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        std::cout << "Invalid framebuffer " << framebufferStatus << std::endl;
    }

    locationViewUniform     = glGetUniformLocation(program, "view");
    locationLightDirection  = glGetUniformLocation(program, "lightDirection");
    locationProjection      = glGetUniformLocation(program, "projection");
    locationModel           = glGetUniformLocation(program, "model");
}

void ShadowPass::renderPre(const glm::mat4 &view_, const glm::mat4 &projection_)
{
    RenderPass::renderPre(view_, projection_);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
}

void ShadowPass::render(const std::vector<const Renderable*>& renderables) const
{
    for (const Renderable* renderable : renderables)
    {
        if (!renderable->material.castsShadow) continue;
        
        VertexBuffer& vertexBuffer = gpuPool.get(renderable);
        vertexBuffer.setMesh(&renderable->mesh);
        vertexBuffer.bind(program);

        setUniforms(*renderable);
    
        glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices().size() * 3, GL_UNSIGNED_INT, 0);
        glCheckError();
    }
}

mat4 ShadowPass::getProjection() const
{
    return perspective<float>(radians(140.0f),1.0f, 1.0f, 5000.0f);
}

GLint ShadowPass::getDepthTexture()
{
    return depthTexture;
}

void ShadowPass::setUniforms(const Renderable &renderable) const
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(getProjection()));

    mat4 model = renderable.object->getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));
}
