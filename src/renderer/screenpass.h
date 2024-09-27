#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "renderer/renderpass.h"
#include "renderer/vertexbuffer.h"
#include "renderer/vertexbufferpool.h"
#include "renderable.h"

class ScreenPass: public RenderPass
{
public:
    ScreenPass(VertexBufferPool& vertexBufferPool);
    ~ScreenPass();

    void init() override;

    void render(const glm::mat4& view, const glm::mat4& projection, const std::vector<const Renderable*>& renderables) const override;

    void setShadow(const glm::mat4& worldToLight, GLint depthTexture);

protected:
    void setUniforms(const Renderable& renderable) const override;

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

    ScreenPass (const ScreenPass&)              = delete;
    ScreenPass& operator= (const ScreenPass&)   = delete;

};