#include "viewport.h"
#include <iostream>

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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
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

    renderPass.init();
    shadowMap.init();
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

    glm::mat4 worldToShadowView = light->getSpace().fromRoot;
    shadowMap.render(worldToShadowView, shadowMap.getProjection(), renderables);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, winWidth, winHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 shadowProjection = shadowMap.getProjection();

    Space projectionSpace = camera->getProjectionSpace((float)winWidth / (float)winHeight);
    renderPass.setShadow(shadowProjection * worldToShadowView, shadowMap.getDepthTexture());
    renderPass.render   (camera->getSpace().fromRoot, camera->getSpace().to(projectionSpace), renderables);

    SDL_GL_SwapWindow(window);
}
