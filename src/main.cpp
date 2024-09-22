#include <emscripten.h>
#include <iostream>

#include "scheduler.h"
#include "viewport.h"

int main(int argc, char** argv)
{
    std::cout << "Graphics engine starting" << std::endl;

    Scheduler  scheduler;
    Viewport   viewport = Viewport(scheduler);

    scheduler.run();

    return 0;
}