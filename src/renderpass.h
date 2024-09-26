#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "renderer.h"
#include "vertexbuffer.h"
#include "renderable.h"
#include "vertexbufferpool.h"

class RenderPass: public Renderer
{
public:
    RenderPass(VertexBufferPool& vertexBufferPool);
    ~RenderPass();

    void init() override;

    void render(float aspectRatio, const glm::mat4& view, const std::vector<const Renderable*>& renderables) const override;

    void setShadow(const glm::mat4& svp, GLint depthTexture);

protected:
    void setUniforms(float aspectRatio, const Renderable& renderable) const override;

    GLint  locationViewUniform;
    GLint  locationLightDirection;
    GLint  locationLightColor;
    GLint  locationRoughness;
    GLint  locationMetallic;
    GLint  locationProjection;
    GLint  locationModel;
    GLint  locationShadowVP;
    GLint  locationDepthTexture;

    glm::mat4   shadowMapViewProjection;
    GLint       depthTexture;

};