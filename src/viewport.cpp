#include "viewport.h"
#include <iostream>

using namespace glm;

Viewport::Viewport(Scheduler& scheduler_)
    : scheduler(scheduler_)
{
    scheduler_.viewports.emplace_back(this);

    std::cout << "create viewport" << std::endl;

    int winWidth = 512, winHeight = 512;
    window = 
        SDL_CreateWindow("Viewport", 
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         winWidth, winHeight, 
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    // Create OpenGLES 2 context on SDL window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GLContext glc = SDL_GL_CreateContext(window);
    printf("INFO: GL version: %s\n", glGetString(GL_VERSION));

    // Set clear color to black
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Get actual GL window size in pixels, in case of high dpi scaling
    SDL_GL_GetDrawableSize(window, &winWidth, &winHeight);
    printf("INFO: GL window size = %dx%d\n", winWidth, winHeight);

    glViewport(0, 0, winWidth, winHeight);

    screenPass.init();
    shadowPass.init();
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

    int winWidth = 512, winHeight = 512;
    SDL_GL_GetDrawableSize(window, &winWidth, &winHeight);

    mat4 worldToLight = light->getSpace().fromRoot;
    shadowPass.renderPre(worldToLight, shadowPass.getProjection());
    shadowPass.render(renderables);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, winWidth, winHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 shadowProjection = shadowPass.getProjection();
    Space projectionSpace = camera->getProjectionSpace((float)winWidth / (float)winHeight);
    
    vec3 lightPosWorld = vec3(light->getSpace().toRoot * vec4(0,0,0,1));
    
    screenPass.setShadow(shadowProjection * worldToLight, lightPosWorld, shadowPass.getDepthTexture());
    screenPass.renderPre(camera->getSpace().fromRoot, camera->getSpace().to(projectionSpace));
    screenPass.render   (renderables);

    SDL_GL_SwapWindow(window);
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
