#include "vertexbuffer.h"
#include "errors.h"
#include <cstddef>

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);
}

void VertexBuffer::bind(GLuint program)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    GLint posAttrib     = glGetAttribLocation(program, "position");
    GLint normalAttrib  = glGetAttribLocation(program, "normal");

    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(normalAttrib);

    glVertexAttribPointer(posAttrib     , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glVertexAttribPointer(normalAttrib  , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

    glCheckError();
}

void VertexBuffer::setMesh(const Mesh* mesh_)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, mesh_->vertices.size() * sizeof(Vertex), mesh_->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_->indices.size() * sizeof(glm::u16vec3), mesh_->indices.data(), GL_STATIC_DRAW);

    mesh = mesh_;
}

const Mesh &VertexBuffer::getMesh() const
{
    return *mesh;
}
