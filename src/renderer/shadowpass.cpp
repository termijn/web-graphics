#include "renderer/shadowpass.h"
#include "iostream"
#include "errors.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "shadowpass.h"

using namespace glm;

ShadowPass::ShadowPass(GpuPool& gpuPool_, DepthTarget& renderTarget_)
    : RenderPass("/package/shaders/shadowmap-vertex.glsl", "/package/shaders/shadowmap-frag.glsl", gpuPool_, renderTarget_)
{

}

ShadowPass::~ShadowPass() = default;

void ShadowPass::init()
{
    RenderPass::init();
    locationViewUniform     = glGetUniformLocation(program, "view");
    locationLightDirection  = glGetUniformLocation(program, "lightDirection");
    locationProjection      = glGetUniformLocation(program, "projection");
    locationModel           = glGetUniformLocation(program, "model");
}

void ShadowPass::renderPre(const glm::mat4 &view_, const glm::mat4 &projection_)
{
    RenderPass::renderPre(view_, projection_);

    glClear (GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}

void ShadowPass::render(const std::vector<const Renderable*>& renderables) const
{
    for (const Renderable* renderable : renderables)
    {
        if (!renderable->material.castsShadow) continue;
        
        VertexBuffer& vertexBuffer = gpuPool.get(renderable);
        vertexBuffer.setMesh(&renderable->mesh);
        vertexBuffer.bind(program);

        setUniforms(*renderable);
    
        glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices().size() * 3, GL_UNSIGNED_INT, 0);
        glCheckError();
    }
}

mat4 ShadowPass::getProjection() const
{
    return perspective<float>(radians(100.0f),1.0f, 1.0f, 1400.0f);
}

void ShadowPass::setUniforms(const Renderable &renderable) const
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(getProjection()));

    mat4 model = renderable.object->getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));
}
