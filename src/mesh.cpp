#include "mesh.h"

using namespace glm;

void Mesh::cube(float size)
{
    float halfSize = size / 2.0f;

    // Define the vertices of the cube, each with the appropriate normal for each face
    vertices = {
        // Back face (-Z)
        { glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4( halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4( halfSize,  halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },
        { glm::vec4(-halfSize,  halfSize, -halfSize, 1.0f), glm::vec4(0, 0, -1, 0) },

        // Front face (+Z)
        { glm::vec4(-halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
        { glm::vec4( halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
        { glm::vec4( halfSize,  halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },
        { glm::vec4(-halfSize,  halfSize,  halfSize, 1.0f), glm::vec4(0, 0, 1, 0) },

        // Left face (-X)
        { glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(-1, 0, 0, 0) },
        { glm::vec4(-halfSize,  halfSize, -halfSize, 1.0f), glm::vec4(-1, 0, 0, 0) },
        { glm::vec4(-halfSize,  halfSize,  halfSize, 1.0f), glm::vec4(-1, 0, 0, 0) },
        { glm::vec4(-halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(-1, 0, 0, 0) },

        // Right face (+X)
        { glm::vec4(halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(1, 0, 0, 0) },
        { glm::vec4(halfSize,  halfSize, -halfSize, 1.0f), glm::vec4(1, 0, 0, 0) },
        { glm::vec4(halfSize,  halfSize,  halfSize, 1.0f), glm::vec4(1, 0, 0, 0) },
        { glm::vec4(halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(1, 0, 0, 0) },

        // Bottom face (-Y)
        { glm::vec4(-halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(0, -1, 0, 0) },
        { glm::vec4( halfSize, -halfSize, -halfSize, 1.0f), glm::vec4(0, -1, 0, 0) },
        { glm::vec4( halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(0, -1, 0, 0) },
        { glm::vec4(-halfSize, -halfSize,  halfSize, 1.0f), glm::vec4(0, -1, 0, 0) },

        // Top face (+Y)
        { glm::vec4(-halfSize, halfSize, -halfSize, 1.0f), glm::vec4(0, 1, 0, 0) },
        { glm::vec4( halfSize, halfSize, -halfSize, 1.0f), glm::vec4(0, 1, 0, 0) },
        { glm::vec4( halfSize, halfSize,  halfSize, 1.0f), glm::vec4(0, 1, 0, 0) },
        { glm::vec4(-halfSize, halfSize,  halfSize, 1.0f), glm::vec4(0, 1, 0, 0) },
    };

    // Define the 12 triangles (6 faces) of the cube using indices
    indices = {
        u16vec3{0, 1, 2}, u16vec3{0, 2, 3},   // Back face
        u16vec3{4, 5, 6}, u16vec3{4, 6, 7},   // Front face
        u16vec3{8, 9, 10}, u16vec3{8, 10, 11}, // Left face
        u16vec3{12, 13, 14}, u16vec3{12, 14, 15}, // Right face
        u16vec3{16, 17, 18}, u16vec3{16, 18, 19}, // Bottom face
        u16vec3{20, 21, 22}, u16vec3{20, 22, 23}  // Top face
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

void Mesh::knot(float radius, float tubeRadius, int segments, int sides)
{
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= segments; ++i)
    {
        float t = i * 2.0f * M_PI / segments;

        // Parametric equations for the knot centerline
        glm::vec3 center = glm::vec3(
            (2.0f + cos(3.0f * t)) * cos(2.0f * t),
            (2.0f + cos(3.0f * t)) * sin(2.0f * t),
            sin(3.0f * t)
        ) * radius;

        // Calculate the derivative (tangent) of the knot for Frenet frame
        glm::vec3 tangent = glm::normalize(glm::vec3(
            -2.0f * sin(2.0f * t) * (2.0f + cos(3.0f * t)) - 6.0f * sin(3.0f * t) * cos(2.0f * t),
            2.0f * cos(2.0f * t) * (2.0f + cos(3.0f * t)) - 6.0f * sin(3.0f * t) * sin(2.0f * t),
            3.0f * cos(3.0f * t)
        ));

        // Find two perpendicular vectors to the tangent to create the Frenet frame
        glm::vec3 bitangent = glm::normalize(glm::cross(tangent, glm::vec3(0.0f, 0.0f, 1.0f)));
        glm::vec3 normal = glm::normalize(glm::cross(tangent, bitangent));

        for (int j = 0; j <= sides; ++j)
        {
            float s = j * 2.0f * M_PI / sides;

            // Compute the position of each vertex around the centerline
            glm::vec3 circlePos = cos(s) * normal + sin(s) * bitangent;

            glm::vec4 position = glm::vec4(center + circlePos * tubeRadius, 1.0f); // thickness of the knot
            glm::vec4 vertexNormal = glm::vec4(glm::normalize(circlePos), 0.0f);

            vertices.push_back({position, vertexNormal});
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
            indices.push_back(glm::u16vec3(i * (sides + 1) + j, nextI * (sides + 1) + j, i * (sides + 1) + nextJ));
            indices.push_back(glm::u16vec3(nextI * (sides + 1) + j, nextI * (sides + 1) + nextJ, i * (sides + 1) + nextJ));
        }
    }
}


void Mesh::sphere(float radius, int rings, int sectors)
{
    const float R = 1.0f / (float)(rings - 1);
    const float S = 1.0f / (float)(sectors - 1);

    // Clear previous data
    vertices.clear();
    indices.clear();

    // Generate vertices
    for (unsigned int r = 0; r < rings; r++) {
        for (unsigned int s = 0; s < sectors; s++) {
            // Calculate the azimuthal angle (longitude) phi and polar angle (latitude) theta
            float theta = M_PI * r * R;       // Range from 0 to pi
            float phi = 2 * M_PI * s * S;     // Range from 0 to 2*pi

            // Spherical to Cartesian conversion
            float x = cos(phi) * sin(theta);
            float y = sin(-M_PI_2 + theta);  // Latitude from -pi/2 to pi/2
            float z = sin(phi) * sin(theta);

            Vertex vertex;
            vertex.position = glm::vec4(x * radius, y * radius, z * radius, 1.0f);
            vertex.normal = glm::vec4(x, y, z, 0.0f); // Normal is the same as the position for a sphere
            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (unsigned int r = 0; r < rings - 1; r++) {
        for (unsigned int s = 0; s < sectors - 1; s++) {
            unsigned int curRow = r * sectors;
            unsigned int nextRow = (r + 1) * sectors;

            indices.push_back(glm::u16vec3(curRow + s, nextRow + s, nextRow + (s + 1)));
            indices.push_back(glm::u16vec3(curRow + s, nextRow + (s + 1), curRow + (s + 1)));
        }
    }
}

