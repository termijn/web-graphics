#pragma once

#include <SDL.h>
#include <SDL_opengles2.h>

#include <chrono>

#include "scheduler.h"

#include "objects/object.h"

#include "renderer/gpupool.h"
#include "renderer/screenpass.h"
#include "renderer/shadowpass.h"

class Renderable;

class Viewport
{
public:
    Viewport(Scheduler& scheduler);
    ~Viewport();

    void attachCamera       (const CameraObject& camera);
    void attachRenderable   (const Renderable&   renderable);
    void attachLight        (const Object&       light);

    void render();

private:
    Scheduler&              scheduler;
    SDL_Window*             window  = nullptr;
    const CameraObject*     camera  = nullptr;
    const Object*           light   = nullptr;

    GpuPool    gpuPool;
    
    ScreenPass          screenPass  = ScreenPass(gpuPool);
    ShadowPass          shadowPass  = ShadowPass(gpuPool);

    std::vector<const Renderable*> renderables;

    Viewport (const Viewport&)              = delete;
    Viewport& operator= (const Viewport&)   = delete;

};
