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
    class ChangeDetector;

    Object();
    explicit Object(const Object& parent);

    virtual ~Object();

    void adopt  (const Object& parent);
    void orphan ();

    void setTransform(const glm::mat4& toParent);

    Space getSpace() const;

    int getRevision() const;

private:
    int             revision    = 0;
    const Object*   parent      = nullptr;
    
    mutable     Space space;
    glm::mat4   toParent    = glm::mat4(1.0);

    mutable std::vector<const Object*> children;

    void updateTransforms() const;
    void removeFromParent();

};

// TODO detects if the transforms from o1 to o2 have changed.
class Object::ChangeDetector
{
public:
    ChangeDetector(const Object& o1, const Object& o2);

    bool isChanged() const;

private:
    std::map<Object*, int> revisionCache;

};