#pragma once

#include <glm/glm.hpp>

#include "viewport.h"
#include "input.h"
#include "objects/object.h"

class RollInput: public Input
{
public:
    RollInput(Viewport& viewport, Object& object_);
    
    void begin(const glm::vec3& position) override;
    void move(const glm::vec3& position) override;
    void end(const glm::vec3& position) override;

private: 
    Object&     object;

};