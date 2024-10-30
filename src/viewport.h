#pragma once

#include <map>

#include <SDL.h>
#include <SDL_opengles2.h>

#include <chrono>
#include <glm/glm.hpp>

#include "scheduler.h"

#include "objects/object.h"

#include "renderer/gpupool.h"
#include "renderer/screenpass.h"
#include "renderer/shadowpass.h"

#include "input.h"

class Renderable;

class Viewport
{
public:
    friend class Scheduler;
    friend class Input;

    Viewport(Scheduler& scheduler);
    ~Viewport();

    void attachCamera       (const CameraObject& camera);
    void attachRenderable   (const Renderable&   renderable);
    void attachLight        (const Object&       light);

    void render();

protected:
    void mouseDown  (MouseButton button, const glm::vec3& position);
    void mouseMove  (const glm::vec3& position);
    void mouseUp    (MouseButton button, const glm::vec3& position);

private:
    Scheduler&              scheduler;
    SDL_Window*             window  = nullptr;
    const CameraObject*     camera  = nullptr;
    const Object*           light   = nullptr;

    GpuPool     gpuPool;
    
    ScreenPass  screenPass  = ScreenPass(gpuPool);
    ShadowPass  shadowPass  = ShadowPass(gpuPool);

    MouseButton pressedButtons = MouseButton::None;

    std::vector<const Renderable*>  renderables;
    std::vector<Input*>       inputs;
    Input* activeInput = nullptr;

    Viewport (const Viewport&)              = delete;
    Viewport& operator= (const Viewport&)   = delete;

};
