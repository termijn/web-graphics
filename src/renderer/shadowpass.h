#pragma once

#include <SDL_opengles2.h>

#include "renderable.h"
#include "renderer/renderpass.h"

class ShadowPass: public RenderPass
{
public:
    ShadowPass(VertexBufferPool& vertexBufferPool);
    ~ShadowPass();

    void init() override;

    void render(const glm::mat4& view, const glm::mat4& projection, const std::vector<const Renderable*>& renderables) const override;

    glm::mat4 getView(const glm::mat4& worldToView) const;
    glm::mat4 getProjection() const;

    GLint getDepthTexture();

protected:
    void setUniforms(const Renderable& renderable) const override;

private:
    int width = 2048;
    int height = 2048;
    
    GLuint depthTexture;
    GLuint framebuffer;

    GLint  locationViewUniform;
    GLint  locationLightDirection;
    GLint  locationProjection;
    GLint  locationModel;

    ShadowPass (const ShadowPass&)              = delete;
    ShadowPass& operator= (const ShadowPass&)   = delete;

};