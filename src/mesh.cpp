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

void Mesh::knot(float radius, int segments, int sides)
{
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= segments; ++i)
    {
        float t = i * 2.0f * M_PI / segments;

        // Parametric equations for the knot
        glm::vec3 center = glm::vec3(
            (2.0f + cos(3.0f * t)) * cos(2.0f * t),
            (2.0f + cos(3.0f * t)) * sin(2.0f * t),
            sin(3.0f * t)
        ) * radius;

        for (int j = 0; j <= sides; ++j)
        {
            float s = j * 2.0f * M_PI / sides;

            // Slightly perturb around the center of the knot to create thickness
            glm::vec3 offset = glm::vec3(
                cos(s),
                sin(s),
                0.0f
            ) * 0.1f; // thickness of the knot

            glm::vec4 position = glm::vec4(center + offset, 1.0f);
            glm::vec4 normal = glm::vec4(glm::normalize(offset), 0.0f);

            vertices.push_back({position, normal});
        }
    }

    // Create indices to form quads or triangles between the segments and sides
    for (int i = 0; i < segments; ++i)
    {
        for (int j = 0; j < sides; ++j)
        {
            int nextI = (i + 1) % segments;
            int nextJ = (j + 1) % sides;

            // Define triangles or quads (two triangles per quad)
            indices.push_back(glm::u16vec3(i * sides + j, nextI * sides + j, i * sides + nextJ));
            indices.push_back(glm::u16vec3(nextI * sides + j, nextI * sides + nextJ, i * sides + nextJ));
        }
    }
}
