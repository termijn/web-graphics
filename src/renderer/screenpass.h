#pragma once

#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "renderer/renderpass.h"
#include "renderer/vertexbuffer.h"
#include "renderer/vertexbufferpool.h"
#include "renderer/texturepool.h"
#include "renderable.h"

class ScreenPass: public RenderPass
{
public:
    ScreenPass(VertexBufferPool& vertexBufferPool, TexturePool& texturePool);
    ~ScreenPass();

    void init() override;

    void renderPre(const glm::mat4& view, const glm::mat4& projection) override;

    void setShadow(const glm::mat4& worldToLight, const glm::vec3& lightPosWorld, GLint depthTexture);

protected:
    void setUniforms(const Renderable& renderable) const override;

    GLint  locationViewUniform;
    GLint  locationLightColor;
    GLint  locationLightPositionWorld;
    GLint  locationBaseColor;
    GLint  locationRoughness;
    GLint  locationMetallic;
    GLint  locationProjection;
    GLint  locationModel;
    GLint  locationShadowVP;
    GLint  locationDepthTexture;
    GLint  locationShaded;
    GLint  locationTextureBaseColor;
    GLint  locationTextureMetallicRoughness;
    GLint  locationHasBaseColorTexture;
    GLint  locationHasMetallicRoughnessTexture;

    glm::vec3   lightPosWorld;
    glm::mat4   shadowMapViewProjection;
    GLint       depthTexture;

    TexturePool& texturePool;

    ScreenPass (const ScreenPass&)              = delete;
    ScreenPass& operator= (const ScreenPass&)   = delete;

};