#include <emscripten.h>

#include "scheduler.h"
#include "viewport.h"

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
    for (Viewport* viewport: viewports)
    {
        viewport->render();
    }
}
