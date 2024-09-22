#include "mesh.h"

void Mesh::cube(float size)
{
     float halfSize = size / 2.0f;

    // Define the 8 vertices of the cube
    vertices = {
        { glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4( halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4( halfSize,  halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4(-halfSize,  halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4(-halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
        { glm::vec4( halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
        { glm::vec4( halfSize,  halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
        { glm::vec4(-halfSize,  halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
    };

    // Define the 12 triangles (6 faces) of the cube using indices
    indices = {
        {0, 1, 2}, {0, 2, 3}, // Back face
        {4, 5, 6}, {4, 6, 7}, // Front face
        {0, 1, 5}, {0, 5, 4}, // Bottom face
        {2, 3, 7}, {2, 7, 6}, // Top face
        {0, 3, 7}, {0, 7, 4}, // Left face
        {1, 2, 6}, {1, 6, 5}, // Right face
    };
}

void Mesh::noisySphere(float radius, int rings, int sectors, float noiseAmplitude)
{
     vertices.clear();
    indices.clear();

    // Generate vertices
    for (int r = 0; r <= rings; ++r) {
        for (int s = 0; s <= sectors; ++s) {
            float phi = M_PI * r / rings; // latitude
            float theta = 2.0f * M_PI * s / sectors; // longitude

            // Compute the position of the vertex on the sphere
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            // Apply noise
            float noise = ((float)rand() / (float) RAND_MAX) * noiseAmplitude; // Random noise in [0, noiseAmplitude]
            x += noise * (rand() % 2 == 0 ? 1 : -1); // Randomly add or subtract noise
            y += noise * (rand() % 2 == 0 ? 1 : -1);
            z += noise * (rand() % 2 == 0 ? 1 : -1);

            glm::vec4 position(x, y, z, 1.0f);
            glm::vec4 normal = glm::normalize(position); // Normal is the normalized position vector

            vertices.push_back({ position, normal });
        }
    }

    // Generate indices
    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            int first = (r * (sectors + 1)) + s;
            int second = first + sectors + 1;

            indices.push_back({ static_cast<glm::u16>(first), static_cast<glm::u16>(second), static_cast<glm::u16>(first + 1) });
            indices.push_back({ static_cast<glm::u16>(second), static_cast<glm::u16>(second + 1), static_cast<glm::u16>(first + 1) });
        }
    }
}
