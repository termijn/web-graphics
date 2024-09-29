#include "renderer/screenpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "screenpass.h"

using namespace glm;

ScreenPass::ScreenPass(VertexBufferPool& vertexBufferPool_)
    : RenderPass("/package/shaders/screenpass-vertex.glsl", "/package/shaders/screenpass-frag.glsl", vertexBufferPool_)
{
}

ScreenPass::~ScreenPass()
{
}

void ScreenPass::init()
{
    RenderPass::init();
    
    locationViewUniform         = glGetUniformLocation(program, "view");
    locationLightColor          = glGetUniformLocation(program, "lightColor");
    locationLightPositionWorld  = glGetUniformLocation(program, "lightPositionWorld");
    locationBaseColor           = glGetUniformLocation(program, "baseColor");
    locationMetallic            = glGetUniformLocation(program, "metallic");
    locationRoughness           = glGetUniformLocation(program, "roughness");
    locationProjection          = glGetUniformLocation(program, "projection");
    locationModel               = glGetUniformLocation(program, "model");
    locationShadowVP            = glGetUniformLocation(program, "shadowVP");
    locationDepthTexture        = glGetUniformLocation(program, "depthTexture");
}

void ScreenPass::renderPre(const glm::mat4 &view, const glm::mat4 &projection)
{
    RenderPass::renderPre(view, projection);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glCullFace(GL_BACK);
}

void ScreenPass::setShadow(const mat4 &worldToLight, const vec3& lightPosWorld_, GLint depthTexture_)
{
    shadowMapViewProjection = worldToLight;
    depthTexture            = depthTexture_;
    lightPosWorld           = lightPosWorld_;
}

void ScreenPass::setUniforms(const Renderable &renderable) const
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform3fv(locationBaseColor, 1, value_ptr(renderable.material.albedo));

    glUniform1f(locationMetallic, renderable.material.metallic);
    glUniform1f(locationRoughness, renderable.material.roughness);

    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(projection));

    glUniform3fv(locationLightPositionWorld, 1, value_ptr(lightPosWorld));

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
