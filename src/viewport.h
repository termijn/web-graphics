#pragma once

#include <map>
#include <chrono>

#include <glm/glm.hpp>

#include "scheduler.h"

#include "objects/object.h"

#include "renderer/gpupool.h"
#include "renderer/screenpass.h"
#include "renderer/shadowpass.h"
#include "renderer/tonemappass.h"
#include "renderer/rendertarget.h"

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
    void mouseWheel (int direction);

private:
    Scheduler&              scheduler;
    const CameraObject*     camera  = nullptr;
    const Object*           light   = nullptr;

    GpuPool     gpuPool;

    CanvasTarget  canvasTarget = CanvasTarget("canvas");
    TextureTarget textureTarget;
    DepthTarget   shadowTarget;

    ShadowPass  shadowPass  = ShadowPass(gpuPool, shadowTarget);
    ScreenPass  screenPass  = ScreenPass(gpuPool, shadowTarget, textureTarget);
    TonemapPass tonemapPass = TonemapPass(gpuPool, textureTarget, canvasTarget);

    MouseButton pressedButtons = MouseButton::None;

    std::vector<const Renderable*>  renderables;
    std::vector<Input*>       inputs;
    Input* activeInput = nullptr;

    Viewport (const Viewport&)              = delete;
    Viewport& operator= (const Viewport&)   = delete;

};
