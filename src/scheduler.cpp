#include <emscripten.h>
#include <iostream>

#include "scheduler.h"
#include "animator.h"
#include "viewport.h"
#include "animator.h"

#include <glm/glm.hpp>

using namespace glm;

void mainLoop(void* mainLoopArg)
{
    Scheduler* scheduler = (Scheduler*) mainLoopArg;
    scheduler->tick();
}

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler() = default;

void Scheduler::run()
{
    void* arg = this;
    emscripten_set_main_loop_arg(mainLoop, arg, 0, true);

    startTime = std::chrono::steady_clock::now();
}

void Scheduler::tick()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;
        vec3 pos(double(mouseX), double(mouseY), 0.0);

        switch (event.type) 
        {
            case SDL_QUIT:
                emscripten_cancel_main_loop();
                break;
            case SDL_MOUSEMOTION:
                {
                    for(auto viewport: viewports)
                        viewport->mouseMove(pos);
                }
                break;
             case SDL_MOUSEBUTTONDOWN:
                {
                    MouseButton button = MouseButton::Left;
                    if (event.button.button == SDL_BUTTON_RIGHT)
                        button = MouseButton::Right;
                    else if (event.button.button == SDL_BUTTON_MIDDLE)
                        button = MouseButton::Middle;

                    for (auto viewport: viewports)
                        viewport->mouseDown(button, pos);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                MouseButton button = MouseButton::Left;
                if (event.button.button == SDL_BUTTON_RIGHT)
                    button = MouseButton::Right;
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                    button = MouseButton::Middle;
                for (auto viewport: viewports)
                    viewport->mouseUp(button, pos);
                break;
        }
    }

    for (Animator* animator: animators)
        animator->animate();

    for (Viewport* viewport: viewports)
        viewport->render();
    
    nrFrames++;

    auto time   = std::chrono::steady_clock::now() - startTime;
    auto milli  = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    if (milli >= 5000 )
    {
        float fps = (float) nrFrames / (milli / 1000.0);
        std::cout << fps << "fps" << std::endl;
        nrFrames = 0;
        startTime = std::chrono::steady_clock::now();
    }
}
