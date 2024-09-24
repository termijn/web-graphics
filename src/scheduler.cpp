#include <emscripten.h>

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
}

void Scheduler::tick()
{
    for (Animator* animator: animators)
        animator->animate();

    for (Viewport* viewport: viewports)
        viewport->render();
}
