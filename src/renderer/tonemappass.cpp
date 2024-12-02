#include "tonemappass.h"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

TonemapPass::TonemapPass(GpuPool &gpuPool_, TextureTarget &hdrTarget_, RenderTarget &renderTarget_)
    : gpuPool(gpuPool_)
    , hdrTarget(hdrTarget_)
    , renderTarget(renderTarget_)
{
}

TonemapPass::~TonemapPass() = default;

void TonemapPass::init()
{
    vertexShader    = gpuPool.compileShader(GL_VERTEX_SHADER, "/package/shaders/tonemappass-vertex.glsl");
    fragmentShader  = gpuPool.compileShader(GL_FRAGMENT_SHADER, "/package/shaders/tonemappass-frag.glsl");

    program = glCreateProgram();
    glAttachShader  (program, vertexShader);
    glAttachShader  (program, fragmentShader);
    glLinkProgram   (program);

    locationHdrTexture      = glGetUniformLocation(program, "hdrTexture");
    locationViewportSize    = glGetUniformLocation(program, "viewportSize");
    locationExposure        = glGetUniformLocation(program, "exposure");

    vertexBuffer.init();

    quad.quad();
    vertexBuffer.setMesh(&quad);

    glUseProgram(program);
    glUniform1i (locationHdrTexture, 9);
}

void TonemapPass::render()
{
    glUseProgram(program);
    renderTarget.beginRender();

    glDisable(GL_CULL_FACE);

    vertexBuffer.bind(program);

    GLuint hdrTexture = hdrTarget.getColorTexture();

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glUniform2fv(locationViewportSize, 1, value_ptr(renderTarget.getSize()));
    glUniform1f (locationExposure, 1.0);

    glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices().size() * 3, GL_UNSIGNED_INT, 0);
}
