#pragma once

#include <SDL_opengles2.h>
#include "mesh.h"

class VertexBuffer
{
public:
    VertexBuffer();

    void init();
    void bind(GLuint program);

    void setMesh(const Mesh* mesh);
    const Mesh& getMesh() const;

private:
    GLuint vertexBufferObject;
    GLuint elementBufferObject;

    const Mesh* mesh;
};