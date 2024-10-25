#include "renderer/screenpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "screenpass.h"

using namespace glm;

ScreenPass::ScreenPass(GpuPool& gpuPool_)
    : RenderPass("/package/shaders/screenpass-vertex.glsl", "/package/shaders/screenpass-frag.glsl", gpuPool_)
{
    nrPoissonSamples = poissonImage.makePoissonDisc(48, 48, 4);
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
    locationPoissonTexture              = glGetUniformLocation(program, "poissonTexture");
    locationNrPoissonSamples            = glGetUniformLocation(program, "nrPoissonSamples");

    locationTexOffset       = glGetUniformLocation(program, "textureTransform.offset");
    locationTexScale        = glGetUniformLocation(program, "textureTransform.scale");
    locationTexRotation     = glGetUniformLocation(program, "textureTransform.rotation");

    locationHasNormalsTexture   = glGetUniformLocation(program, "normalMap.hasNormalMap");
    locationNormalsTexture      = glGetUniformLocation(program, "normalMap.texture");
    locationNormalMapScale      = glGetUniformLocation(program, "normalMap.scale");

    locationHasOcclusionMap     = glGetUniformLocation(program, "occlusion.hasOcclusionMap");
    locationOcclusionMap        = glGetUniformLocation(program, "occlusion.occlusionMap");

    locationHasEmmissiveTexture = glGetUniformLocation(program, "emissive.hasEmissiveTexture");
    locationEmissiveTexture     = glGetUniformLocation(program, "emissive.emissiveTexture");
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
    const Material& material = renderable.material;

    glUniform1i(locationShaded, material.shaded ? 1 : 0);
    
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform3fv(locationBaseColor, 1, value_ptr(material.albedo));

    glUniform1f(locationMetallic, material.metallic);
    glUniform1f(locationRoughness, material.roughness);

    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(projection));

    glUniform3fv(locationLightPositionWorld, 1, value_ptr(lightPosWorld));

    mat4 model = renderable.object->getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));

    glUniformMatrix4fv(locationShadowVP, 1, GL_FALSE, value_ptr(shadowMapViewProjection));

    glUniform1i(locationDepthTexture, 0);

    if (material.baseColorTexture.has_value()) 
    {
        Texture& texture = gpuPool.get(&material.baseColorTexture.value(), Texture::Interpolation::Linear);
        glUniform1i(locationTextureBaseColor, 1);
        glUniform1i(locationHasBaseColorTexture, 1);
        texture.bind(GL_TEXTURE1);
    } 
    else
    {
        glUniform1i(locationHasBaseColorTexture, 0); // Indicate there is no base color texture
    }

    if (material.metallicRoughness.has_value())
    {
        Texture& texture = gpuPool.get(&material.metallicRoughness.value(), Texture::Interpolation::Nearest);
        glUniform1i(locationTextureMetallicRoughness, 2);
        glUniform1i(locationHasMetallicRoughnessTexture, 1);
        texture.bind(GL_TEXTURE2);
    } 
    else
    {
        glUniform1i(locationHasMetallicRoughnessTexture, 0);
    }

    Texture& poissonTexture = gpuPool.get(&poissonImage, Texture::Interpolation::Nearest);
    glUniform1i(locationPoissonTexture, 3);
    poissonTexture.bind(GL_TEXTURE3);

    glUniform1i(locationNrPoissonSamples, nrPoissonSamples);

    vec2 baseColorOffset = material.baseColorTransforms.offset;
    glUniform2f(locationTexOffset, baseColorOffset.x, baseColorOffset.y);

    vec2 baseColorScale = material.baseColorTransforms.scale;
    glUniform2f(locationTexScale, baseColorScale.x, baseColorScale.y);

    glUniform1f(locationTexRotation , material.baseColorTransforms.rotation);

    if (material.normalMap.has_value()) 
    {
        Texture& texture = gpuPool.get(&material.normalMap.value(), Texture::Interpolation::Nearest);
        glUniform1i(locationNormalsTexture, 4);
        texture.bind(GL_TEXTURE4);

        glUniform1f(locationNormalMapScale, material.normalMapScale);
    }
    glUniform1i(locationHasNormalsTexture, material.normalMap.has_value() ? 1 : 0);

    if (material.occlusion.has_value())
    {
        Texture& texture = gpuPool.get(&material.occlusion.value(), Texture::Interpolation::Linear);
        glUniform1i(locationOcclusionMap, 5);
        texture.bind(GL_TEXTURE5);
    }
    glUniform1i(locationHasOcclusionMap, material.occlusion.has_value() ? 1 : 0);

    if (material.emissive.has_value())
    {
        Texture& texture = gpuPool.get(&material.emissive.value(), Texture::Interpolation::Linear);
        glUniform1i(locationEmissiveTexture, 6);
        texture.bind(GL_TEXTURE6);
    }
    glUniform1i(locationHasEmmissiveTexture, material.emissive.has_value() ? 1 : 0);
}
