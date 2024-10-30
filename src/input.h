#pragma once

#include <glm/glm.hpp>

class Viewport;

enum class MouseButton 
{
    None    = 0x00,
    Left    = 0x01,
    Middle  = 0x02,
    Right   = 0x04
};

class Input
{
public:
    Input(Viewport& viewport);
    virtual ~Input();

    MouseButton button;

    virtual bool query  (const glm::vec3& position, MouseButton button);

    virtual void begin  (const glm::vec3& position);
    virtual void move   (const glm::vec3& position);
    virtual void end    (const glm::vec3& position);

    glm::vec3 delta() const;
    glm::vec3 deltaRelative() const;

private:
    Viewport& viewport;
    glm::vec3 beginPos;
    glm::vec3 lastPos;
    glm::vec3 lastDelta;

};