#pragma once

#include <vector>
#include <glm/glm.hpp>

class Box
{
public:
    glm::vec3 min = glm::vec3(900000000.0f);
    glm::vec3 max = glm::vec3(-900000000.0f);

    void expand(const glm::vec3& point);

    glm::vec3 center() const;
};

class Vertex
{
public:
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec2 uv;
    glm::vec4 tangent    = glm::vec4(0.0);
    glm::vec4 bitangent  = glm::vec4(0.0);

};

// TODO: Add shallow copying to facilitate instanced meshes.
class Mesh
{
public:

    std::vector<Vertex>         vertices;
    std::vector<glm::u32vec3>   indices;

    Box boundingBox;

    void generateNormals();
    void generateBoundingBox();
    void generateTangentVectors();

    void cube       (float size);
    void noisySphere(float radius, int rings, int sectors, float noiseAmplitude);
    void knot       (float radius, float tubeRadius, int segments, int sides);
    void sphere(float radius, int rings, int sectors);
    
};