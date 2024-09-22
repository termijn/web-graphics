#pragma once

#include <vector>

class Viewport;

class Scheduler
{
friend class Viewport;

public:
    Scheduler();
    virtual ~Scheduler();

    void run();

    void tick();

private:
    std::vector<Viewport*> viewports;
};