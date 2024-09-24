#pragma once

#include <SDL.h>
#include <SDL_opengles2.h>

#include <chrono>

#include "scheduler.h"
#include "vertexbuffer.h"
#include "renderpass.h"
#include "objects/object.h"

class Renderable;

class Viewport
{
public:
    Viewport(Scheduler& scheduler);
    ~Viewport();

    void attachCamera(Object& camera);
    void attachRenderable(const Renderable& renderable);

    void render();

private:
    Scheduler&      scheduler;
    SDL_Window*     window = nullptr;
    Object*         camera = nullptr;

    RenderPass      renderPass;

    std::vector<const Renderable*> renderables;
};
