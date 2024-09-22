#pragma once

#include <cmath>
#include <vector>
#include <SDL_opengles2.h>
#include <glm/glm.hpp>
#include "mesh.h"

void generateKnot(Mesh& mesh, int segments, int rings, float radius, float tubeRadius) 
{
    // Clear the mesh data
    mesh.vertices.clear();
    mesh.indices.clear();

    // Generate vertices and normals
    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / (float)segments * 2.0f * M_PI;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (int j = 0; j <= rings; ++j) {
            float phi = (float)j / (float)rings * 2.0f * M_PI;
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            // Torus knot formula
            float r = radius + tubeRadius * cosPhi;
            glm::vec4 vertex(r * cosTheta, r * sinTheta, tubeRadius * sinPhi, 1);

            // Normal vector (approximated)
            glm::vec4 normal(cosPhi * cosTheta, cosPhi * sinTheta, sinPhi, 0);
            normal = glm::normalize(normal);  // Normalize the normal

            // Add vertex and normal to the mesh
            mesh.vertices.push_back(Vertex { .position = vertex, .normal = normal });
        }
    }

    // Generate indices
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < rings; ++j) {
            GLuint first = (i * (rings + 1)) + j;
            GLuint second = first + rings + 1;

            // Store each triangle as three indices
            mesh.indices.push_back(glm::u16vec3(first, second, first + 1));
            mesh.indices.push_back(glm::u16vec3(second, second + 1, first + 1));
        }
    }
}
