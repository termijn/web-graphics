#pragma once

#include <vector>
#include <glm/glm.hpp>

class Vertex
{
public:
    glm::vec4 position;
    glm::vec4 normal;
};

class Mesh
{
public:
    std::vector<Vertex>         vertices;
    std::vector<glm::u16vec3>   indices;

    void cube(float size);
    void noisySphere(float radius, int rings, int sectors, float noiseAmplitude);
    void knot(float radius, int segments, int sides);
};