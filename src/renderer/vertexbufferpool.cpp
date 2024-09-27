#include "renderer/vertexbufferpool.h"

VertexBufferPool::VertexBufferPool()
{
}

VertexBufferPool::~VertexBufferPool()
{
}

VertexBuffer &VertexBufferPool::get(const Renderable *renderable)
{
    if (!pool.contains(renderable))
    {
        pool.emplace(renderable, VertexBuffer());
        pool[renderable].init();
    }
    return pool[renderable];
}
