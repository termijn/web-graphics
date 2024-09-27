#include "renderer/screenpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

ScreenPass::ScreenPass(VertexBufferPool& vertexBufferPool_)
    : RenderPass("/shaders/screenpass-vertex.glsl", "/shaders/screenpass-frag.glsl", vertexBufferPool_)
{
}

ScreenPass::~ScreenPass()
{
}

void ScreenPass::init()
{
    RenderPass::init();
    
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

void ScreenPass::render(const mat4& view_, const mat4& projection_, const std::vector<const Renderable *> &renderables) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    RenderPass::render(view_, projection_, renderables);
}

void ScreenPass::setShadow(const mat4 &worldToLight, GLint depthTexture_)
{
    shadowMapViewProjection = worldToLight;
    depthTexture            = depthTexture_;
}

void ScreenPass::setUniforms(const Renderable &renderable) const
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    vec4 lightDir(-0.3, -0.3, -1.0, 0.0);
    lightDir = lightDir;

    glUniform3f(locationLightDirection, lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform1f(locationMetallic, renderable.material.metallic);
    glUniform1f(locationRoughness, renderable.material.roughness);

    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(projection));

    mat4 model = renderable.object.getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));

    // mat4 shadowToTexture(
    //     0.5, 0.0, 0.0, 0.0,
    //     0.0, 0.5, 0.0, 0.0,
    //     0.0, 0.0, 0.5, 0.0,
    //     0.5, 0.5, 0.5, 1.0
    //     );
    // mat4 toTextureSpace = shadowToTexture * shadowMapViewProjection;
    glUniformMatrix4fv(locationShadowVP, 1, GL_FALSE, value_ptr(shadowMapViewProjection));

    glUniform1i(locationDepthTexture, 0);  // Assign texture unit 0 to the sampler
}
