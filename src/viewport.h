#pragma once

#include <SDL.h>
#include <SDL_opengles2.h>

#include <chrono>

#include "scheduler.h"
#include "vertexbuffer.h"
#include "renderpass.h"

class Viewport
{
public:
    Viewport(Scheduler& scheduler);
    ~Viewport();

    void render();

private:
    Scheduler& scheduler;

    std::chrono::time_point<std::chrono::steady_clock> startTime;

    SDL_Window*     window;
    
    Mesh            mesh;
    VertexBuffer    vertexBuffer;
    RenderPass      renderPass  = RenderPass(vertexBuffer);

};
