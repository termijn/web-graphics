#pragma once

#include <vector>
#include <glm/glm.hpp>

class Vertex
{
public:
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 uv;
};

class Mesh
{
public:

    std::vector<Vertex>         vertices;
    std::vector<glm::u16vec3>   indices;

    void generateNormals();

    void cube       (float size);
    void noisySphere(float radius, int rings, int sectors, float noiseAmplitude);
    void knot       (float radius, float tubeRadius, int segments, int sides);
    void sphere(float radius, int rings, int sectors);
    
};