#include "shadowmap.h"
#include "iostream"
#include "errors.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

ShadowMap::ShadowMap(VertexBufferPool& vertexBufferPool_)
    : Renderer("/shaders/shadowmap-vertex.glsl", "/shaders/shadowmap-frag.glsl", vertexBufferPool_)
{

}

ShadowMap::~ShadowMap() = default;

void ShadowMap::init()
{
    Renderer::init();

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
        GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER); // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMEN(0x8CD6)
        std::cout << "Invalid framebuffer " << framebufferStatus << std::endl;
    }

    locationViewUniform     = glGetUniformLocation(program, "view");
    locationLightDirection  = glGetUniformLocation(program, "lightDirection");
    locationProjection      = glGetUniformLocation(program, "projection");
    locationModel           = glGetUniformLocation(program, "model");
}

void ShadowMap::render(const glm::mat4& view_, const glm::mat4& projection_, const std::vector<const Renderable*>& renderables) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);

    Renderer::render(view_, projection_, renderables);
}

glm::mat4 ShadowMap::getProjection() const
{
    return glm::perspective<float>(glm::radians(90.0f),1.0f, 1.0f, 200.0f);
}

GLint ShadowMap::getDepthTexture()
{
    return depthTexture;
}

void ShadowMap::setUniforms(const Renderable &renderable) const
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(getProjection()));

    glm::mat4 model = renderable.object.getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));
}
