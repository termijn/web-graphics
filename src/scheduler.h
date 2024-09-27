#pragma once

#include <vector>
#include <chrono>

class Viewport;
class Animator;

class Scheduler
{
friend class Viewport;
friend class Animator;

public:
    Scheduler();
    virtual ~Scheduler();

    void run();

    void tick();

private:
    std::vector<Viewport*> viewports;
    std::vector<Animator*> animators;

    std::chrono::time_point<std::chrono::steady_clock> startTime;
    int nrFrames = 0;

    Scheduler (const Scheduler&)              = delete;
    Scheduler& operator= (const Scheduler&)   = delete;
};