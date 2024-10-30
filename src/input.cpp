#include "input.h"
#include "viewport.h"

using namespace glm;

Input::Input(Viewport &viewport_)
    : viewport(viewport_)
{
    viewport.inputs.emplace_back(this);
    button = MouseButton::Left;
}

Input::~Input()
{
    auto it = std::find(viewport.inputs.begin(), viewport.inputs.end(), this);
    viewport.inputs.erase(it);
}

bool Input::query(const vec3 &position, MouseButton button_)
{
    return button == button_;
}

void Input::begin(const vec3 &position)
{
    beginPos = position;
    lastDelta = vec3(0.0);
    lastPos = position;
}

void Input::move(const vec3 &position)
{
    lastDelta = lastPos - position;
    lastPos = position;
}

void Input::end(const vec3 &position)
{
    lastDelta = lastPos - position;
    lastPos = position;
}

vec3 Input::delta() const
{
    return lastPos - beginPos;
}

vec3 Input::deltaRelative() const
{
    return lastDelta;
}
