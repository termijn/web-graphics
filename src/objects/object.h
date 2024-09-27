#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>

class Space
{
public:
    glm::mat4 toRoot    = glm::mat4(1.0);
    glm::mat4 fromRoot  = glm::mat4(1.0);

    glm::mat4 to            (const Space& target) const;
    glm::vec3 transformPos  (const glm::vec3& position,     const Space& targetSpace) const;
    glm::vec3 transformDir  (const glm::vec3& direction,    const Space& targetSpace) const;

};

class Object
{
public:
    Object();
    explicit Object(const Object& parent);

    virtual ~Object();

    void adopt  (const Object& parent);
    void orphan ();

    void setTransform(const glm::mat4& toParent);

    Space getSpace() const;

private:
    const Object*   parent      = nullptr;
    
    mutable Space   space;
    glm::mat4       toParent    = glm::mat4(1.0);

    mutable std::vector<const Object*> children;

    void updateTransforms() const;
    void removeFromParent();

    Object& operator= (const Object&)   = delete;

};
