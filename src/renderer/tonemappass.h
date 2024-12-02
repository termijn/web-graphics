#pragma once

#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "renderer/vertexbuffer.h"
#include "renderer/gpupool.h"
#include "renderer/rendertarget.h"

class TonemapPass
{
public:
    TonemapPass(GpuPool& gpuPool, TextureTarget& hdrTarget, RenderTarget& renderTarget);
    ~TonemapPass();

    void init();
    void render();

protected:
    GpuPool&        gpuPool;
    TextureTarget&  hdrTarget;
    RenderTarget&   renderTarget;
    
    GLuint        vertexShader;
    GLuint        fragmentShader;
    GLuint        program;

    GLuint        locationHdrTexture;
    GLuint        locationViewportSize;
    GLuint        locationExposure;

    Mesh quad;
    VertexBuffer vertexBuffer;

    TonemapPass             (const TonemapPass&)   = delete;
    TonemapPass& operator=  (const TonemapPass&)   = delete;

};