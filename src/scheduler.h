#pragma once

#include <vector>

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
};