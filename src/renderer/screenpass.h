#pragma once

#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "renderer/renderpass.h"
#include "renderer/vertexbuffer.h"
#include "renderer/gpupool.h"
#include "renderable.h"
#include "image.h"

class ScreenPass: public RenderPass
{
public:
    ScreenPass(GpuPool& gpuPool);
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
    GLint  locationPoissonTexture;
    GLint  locationNrPoissonSamples;

    GLint  locationNormalsTexture;
    GLint  locationHasNormalsTexture;
    GLint  locationNormalMapScale;

    GLint locationHasOcclusionMap;
    GLint locationOcclusionMap;

    GLint locationHasEmmissiveTexture;
    GLint locationEmissiveTexture;

    GLint locationTexOffset;
    GLint locationTexScale;
    GLint locationTexRotation;

    glm::vec3   lightPosWorld;
    glm::mat4   shadowMapViewProjection;
    GLint       depthTexture;

    GLint       poissonTexture;
    int         nrPoissonSamples;

    Image poissonImage;

    ScreenPass (const ScreenPass&)              = delete;
    ScreenPass& operator= (const ScreenPass&)   = delete;

};