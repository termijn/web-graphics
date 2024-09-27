#pragma once

#include "vertexbuffer.h"
#include "renderable.h"
#include <map>

class VertexBufferPool
{
public:
    VertexBufferPool();
    ~VertexBufferPool();

    VertexBuffer& get(const Renderable* renderable);

private:
    std::map<const Renderable*, VertexBuffer> pool;

    VertexBufferPool (const VertexBufferPool&)              = delete;
    VertexBufferPool& operator= (const VertexBufferPool&)   = delete;

};