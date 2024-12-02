#include "viewport.h"
#include <iostream>

using namespace glm;

Viewport::Viewport(Scheduler& scheduler_)
    : scheduler(scheduler_)
{
    scheduler_.viewports.emplace_back(this);

    screenPass.init();
    shadowPass.init();
    tonemapPass.init();
}

Viewport::~Viewport()
{
    auto it = std::find(scheduler.viewports.begin(), scheduler.viewports.end(), this);
    scheduler.viewports.erase(it);
}

void Viewport::attachCamera(const CameraObject& camera_)
{
    camera = &camera_;
}

void Viewport::attachRenderable(const Renderable& renderable)
{
    renderables.emplace_back(&renderable);
}

void Viewport::attachLight(const Object &light_)
{
    light = &light_;
}

void Viewport::render()
{
    if (camera == nullptr) return; // Nothing to render without a camera

    mat4 worldToLight = light->getSpace().fromRoot;
    shadowPass.renderPre(worldToLight, shadowPass.getProjection());
    shadowPass.render(renderables);

    vec2 size = canvasTarget.getSize();
    textureTarget.setSize(size);

    mat4 shadowProjection = shadowPass.getProjection();
    Space projectionSpace = camera->getProjectionSpace(size.x / size.y);
    
    vec3 lightPosWorld = vec3(light->getSpace().toRoot * vec4(0,0,0,1));
    
    screenPass.setShadow(shadowProjection * worldToLight, lightPosWorld);
    screenPass.renderPre(camera->getSpace().fromRoot, camera->getSpace().to(projectionSpace));
    screenPass.render   (renderables);

    tonemapPass.render();

    canvasTarget.endRender();
}

void Viewport::mouseDown(MouseButton button, const glm::vec3& position)
{
    std::cout << "mouseDown" << std::endl;
    pressedButtons = static_cast<MouseButton>(static_cast<unsigned int>(pressedButtons) | static_cast<unsigned int>(button));

    if (activeInput != nullptr)
        activeInput->end(position);

    activeInput = nullptr;

    for(auto input: inputs)
    {
        if (input->query(position, pressedButtons))
        {
            activeInput = input;
            activeInput->begin(position);
        }
    }
}

void Viewport::mouseMove(const glm::vec3 &position)
{
    std::cout << "mouseMove" << std::endl;

    if (activeInput == nullptr)
    {
        for(auto input: inputs)
        {
            if (input->query(position, pressedButtons))
                activeInput = input;
        }
    } 
    else 
    {
        activeInput->move(position);
    }
}

void Viewport::mouseUp(MouseButton button, const glm::vec3& position)
{
    std::cout << "mouseUp" << std::endl;
    pressedButtons = static_cast<MouseButton>(static_cast<unsigned int>(pressedButtons) & ~static_cast<unsigned int>(button));

    if (activeInput != nullptr)
    {
        activeInput->end(position);
        activeInput = nullptr;
    }
}

void Viewport::mouseWheel(int direction)
{
    for(auto input: inputs)
    {
        input->mouseWheel(direction);
    }
}
