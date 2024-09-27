#pragma once

#include <SDL_opengles2.h>

#include "renderable.h"
#include "renderer.h"

class ShadowMap: public Renderer
{
public:
    ShadowMap(VertexBufferPool& vertexBufferPool);
    ~ShadowMap();

    void init() override;

    void render(float aspectRatio, const glm::mat4& view, const std::vector<const Renderable*>& renderables) const override;

    glm::mat4 getView(const glm::mat4& worldToView);
    glm::mat4 getProjection();

    GLint getDepthTexture();

protected:
    void setUniforms(float aspectRatio, const Renderable& renderable) const override;

private:
    int width = 2048;
    int height = 2048;
    
    GLuint depthTexture;
    GLuint framebuffer;

    GLint  locationViewUniform;
    GLint  locationLightDirection;
    GLint  locationProjection;
    GLint  locationModel;

    ShadowMap (const ShadowMap&)              = delete;
    ShadowMap& operator= (const ShadowMap&)   = delete;

};