#pragma once

#include <SDL_opengles2.h>

#include "renderable.h"
#include "renderer/renderpass.h"
#include "renderer/rendertarget.h"

class ShadowPass: public RenderPass
{
public:
    ShadowPass(GpuPool& gpuPool, DepthTarget& renderTarget);
    ~ShadowPass();

    void init() override;
    
    void renderPre(const glm::mat4& view, const glm::mat4& projection) override;
    void render(const std::vector<const Renderable*>& renderables) const override;

    glm::mat4 getView(const glm::mat4& worldToView) const;
    glm::mat4 getProjection() const;

    GLint getDepthTexture();

protected:
    void setUniforms(const Renderable& renderable) const override;

private:
    int width  = 2048;
    int height = 2048;
    
    GLint  locationViewUniform;
    GLint  locationLightDirection;
    GLint  locationProjection;
    GLint  locationModel;

    ShadowPass (const ShadowPass&)              = delete;
    ShadowPass& operator= (const ShadowPass&)   = delete;

};