#include "roll.h"

#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

RollInput::RollInput(Viewport& viewport, Object& object_)
        : Input(viewport)
        , object(object_)
{
}

void RollInput::begin(const vec3& position)
{
    Input::begin(position);
}

void RollInput::move(const vec3& position)
{
    Input::move(position);
    vec2 rotation = deltaRelative() * 0.1f;

    vec3 center = Space::pos(vec3(0.0), Space(), object.getParentSpace());

    mat4 newTransform = object.getTransform();
    
    vec3 yAxis = Space::dir(vec3(0, 1, 0), object.getSpace(), object.getParentSpace());
    vec3 xAxis = Space::dir(vec3(1, 0, 0), object.getSpace(), object.getParentSpace());

    newTransform = translate    (mat4(1.0), -center) * newTransform;
    newTransform = rotate       (mat4(1.0), radians(rotation.x), yAxis) * newTransform;
    newTransform = rotate       (mat4(1.0), radians(rotation.y), xAxis) * newTransform;
    newTransform = translate    (mat4(1.0), center) * newTransform;

    object.setTransform(newTransform);
}

void RollInput::end(const vec3& position)
{
    Input::end(position);
}
