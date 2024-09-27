#include <emscripten.h>
#include <iostream>

#include "scheduler.h"
#include "animator.h"
#include "viewport.h"
#include "animator.h"

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
