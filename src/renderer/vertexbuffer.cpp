#include "renderer/vertexbuffer.h"
#include "errors.h"
#include <cstddef>

VertexBuffer::VertexBuffer()
{
}

void VertexBuffer::init()
{
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);
    glCheckError();
}

void VertexBuffer::bind(GLuint program)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    GLint posAttrib         = glGetAttribLocation(program, "position");
    GLint normalAttrib      = glGetAttribLocation(program, "normal");
    GLint uvAttrib          = glGetAttribLocation(program, "uv");
    GLint tangentAttrib     = glGetAttribLocation(program, "tangent");
    GLint bitangentAttrib   = glGetAttribLocation(program, "bitangent");

    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(posAttrib     , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

    if (normalAttrib > -1)
    {
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(normalAttrib  , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    }

    if (uvAttrib > -1)
    {
        glEnableVertexAttribArray(uvAttrib);
        glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));
    }

    if (tangentAttrib > -1)
    {
        glEnableVertexAttribArray(tangentAttrib);
        glVertexAttribPointer(tangentAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
    }

    if (bitangentAttrib > -1)
    {
        glEnableVertexAttribArray(bitangentAttrib);
        glVertexAttribPointer(bitangentAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
    }
    
    glCheckError();
}

void VertexBuffer::setMesh(const Mesh* mesh_)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, mesh_->vertices.size() * sizeof(Vertex), mesh_->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_->indices.size() * sizeof(glm::u32vec3), mesh_->indices.data(), GL_STATIC_DRAW);

    mesh = mesh_;
}

const Mesh &VertexBuffer::getMesh() const
{
    return *mesh;
}
