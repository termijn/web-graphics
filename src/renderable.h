#pragma once

#include <glm/glm.hpp>
#include "objects/object.h"
#include "mesh.h"

class Material
{
public:
    glm::vec3   albedo      = glm::vec3(0.0,0.5,0.6);
    float       roughness   = 0.6;
    float       metallic    = 0.2;

    bool        castsShadow = true;
};

class Renderable
{
public:
    Renderable(Object& object);
    ~Renderable();

    Object&     object;
    Material    material;
    Mesh        mesh;

    Renderable (const Renderable&)              = delete;
    Renderable& operator= (const Renderable&)   = delete;

};
