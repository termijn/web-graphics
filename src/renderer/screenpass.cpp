#include "renderer/screenpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "screenpass.h"

using namespace glm;

ScreenPass::ScreenPass(VertexBufferPool& vertexBufferPool_, TexturePool& texturePool_)
    : RenderPass("/package/shaders/screenpass-vertex.glsl", "/package/shaders/screenpass-frag.glsl", vertexBufferPool_)
    , texturePool(texturePool_)
{
}

ScreenPass::~ScreenPass()
{
}

void ScreenPass::init()
{
    RenderPass::init();
    
    locationViewUniform              = glGetUniformLocation(program, "view");
    locationLightColor               = glGetUniformLocation(program, "lightColor");
    locationLightPositionWorld       = glGetUniformLocation(program, "lightPositionWorld");
    locationBaseColor                = glGetUniformLocation(program, "baseColor");
    locationMetallic                 = glGetUniformLocation(program, "metallic");
    locationRoughness                = glGetUniformLocation(program, "roughness");
    locationProjection               = glGetUniformLocation(program, "projection");
    locationModel                    = glGetUniformLocation(program, "model");
    locationShadowVP                 = glGetUniformLocation(program, "shadowVP");
    locationDepthTexture             = glGetUniformLocation(program, "depthTexture");
    locationShaded                   = glGetUniformLocation(program, "shaded");

    locationTextureBaseColor         = glGetUniformLocation(program, "baseColorTexture");
    locationTextureMetallicRoughness = glGetUniformLocation(program, "metallicRoughnessTexture");

    locationHasBaseColorTexture         = glGetUniformLocation(program, "hasBaseColorTexture");
    locationHasMetallicRoughnessTexture = glGetUniformLocation(program, "hasMetallicRoughnessTexture");
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
    glUniform1i(locationShaded, renderable.material.shaded ? 1 : 0);
    
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform3fv(locationBaseColor, 1, value_ptr(renderable.material.albedo));

    glUniform1f(locationMetallic, renderable.material.metallic);
    glUniform1f(locationRoughness, renderable.material.roughness);

    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(projection));

    glUniform3fv(locationLightPositionWorld, 1, value_ptr(lightPosWorld));

    mat4 model = renderable.object->getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));

    // mat4 shadowToTexture(
    //     0.5, 0.0, 0.0, 0.0,
    //     0.0, 0.5, 0.0, 0.0,
    //     0.0, 0.0, 0.5, 0.0,
    //     0.5, 0.5, 0.5, 1.0
    //     );
    // mat4 toTextureSpace = shadowToTexture * shadowMapViewProjection;
    glUniformMatrix4fv(locationShadowVP, 1, GL_FALSE, value_ptr(shadowMapViewProjection));

    glUniform1i(locationDepthTexture, 0);

    if (renderable.material.baseColorTexture.has_value()) 
    {
        Texture& texture = texturePool.get(&renderable.material.baseColorTexture.value());
        glUniform1i(locationTextureBaseColor, 1);
        glUniform1i(locationHasBaseColorTexture, 1);
        texture.bind(GL_TEXTURE1);
    } 
    else
    {
        glUniform1i(locationHasBaseColorTexture, 0); // Indicate there is no base color texture
    }

    if (renderable.material.metallicRoughness.has_value())
    {
        Texture& texture = texturePool.get(&renderable.material.metallicRoughness.value());
        glUniform1i(locationTextureMetallicRoughness, 2);
        glUniform1i(locationHasMetallicRoughnessTexture, 1);
        texture.bind(GL_TEXTURE2);
    } 
    else
    {
        glUniform1i(locationHasMetallicRoughnessTexture, 0);
    }
}
