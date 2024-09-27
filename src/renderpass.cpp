#include "renderpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

RenderPass::RenderPass(VertexBufferPool& vertexBufferPool_)
    : Renderer("/shaders/vertex.glsl", "/shaders/frag.glsl", vertexBufferPool_)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::init()
{
    Renderer::init();
    
    locationViewUniform     = glGetUniformLocation(program, "view");
    locationLightColor      = glGetUniformLocation(program, "lightColor");
    locationLightDirection  = glGetUniformLocation(program, "lightDirection");
    locationMetallic        = glGetUniformLocation(program, "metallic");
    locationRoughness       = glGetUniformLocation(program, "roughness");
    locationProjection      = glGetUniformLocation(program, "projection");
    locationModel           = glGetUniformLocation(program, "model");
    locationShadowVP        = glGetUniformLocation(program, "shadowVP");
    locationDepthTexture    = glGetUniformLocation(program, "depthTexture");
}

void RenderPass::render(const glm::mat4& view_, const glm::mat4& projection_, const std::vector<const Renderable *> &renderables) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    Renderer::render(view_, projection_, renderables);
}

void RenderPass::setShadow(const glm::mat4 &worldToLight, GLint depthTexture_)
{
    shadowMapViewProjection = worldToLight;
    depthTexture            = depthTexture_;
}

void RenderPass::setUniforms(const Renderable &renderable) const
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    glm::vec4 lightDir(-0.3, -0.3, -1.0, 0.0);
    lightDir = lightDir;

    glUniform3f(locationLightDirection, lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform1f(locationMetallic, renderable.material.metallic);
    glUniform1f(locationRoughness, renderable.material.roughness);

    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(projection));

    glm::mat4 model = renderable.object.getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));

    // glm::mat4 shadowToTexture(
    //     0.5, 0.0, 0.0, 0.0,
    //     0.0, 0.5, 0.0, 0.0,
    //     0.0, 0.0, 0.5, 0.0,
    //     0.5, 0.5, 0.5, 1.0
    //     );
    // glm::mat4 toTextureSpace = shadowToTexture * shadowMapViewProjection;
    glUniformMatrix4fv(locationShadowVP, 1, GL_FALSE, value_ptr(shadowMapViewProjection));

    glUniform1i(locationDepthTexture, 0);  // Assign texture unit 0 to the sampler
}
