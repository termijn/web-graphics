#include "object.h"

Object::Object()
{
}

Object::Object(const Object &parent_)
    : parent(&parent_)
{
    parent_.children.emplace_back(&parent_);
}

void Object::adopt(const Object& parent_)
{
    removeFromParent();

    parent = &parent_;
    parent->children.emplace_back(this);

    updateTransforms();
}

void Object::orphan()
{
   removeFromParent();
   updateTransforms();
}

void Object::setTransform(const glm::mat4 &toParent_)
{
    toParent = toParent_;
    updateTransforms();
}

Space Object::getSpace() const
{
    return space;
}

void Object::updateTransforms() const
{
    if (parent == nullptr)
    {
        space.toRoot      = toParent;
        space.fromRoot    = glm::inverse(toParent);
    }
    else
    {
        space.toRoot      = parent->space.toRoot * toParent;
        space.fromRoot    = glm::inverse(space.toRoot);
    }

    for(auto child : children)
        child->updateTransforms();
}

void Object::removeFromParent()
{
    if (parent != nullptr)
    {
        auto it = std::find(parent->children.begin(), parent->children.end(), parent);
        parent->children.erase(it);
    }
    parent = nullptr;
}

glm::mat4 Space::to(const Space &target) const
{
    return target.fromRoot * toRoot;
}

glm::vec3 Space::transformPos(const glm::vec3 &position, const Space &targetSpace) const
{
    return to(targetSpace) * glm::vec4(position, 1.0);
}

glm::vec3 Space::transformDir(const glm::vec3 &direction, const Space &targetSpace) const
{
    return to(targetSpace) * glm::vec4(direction, 0.0);
}
