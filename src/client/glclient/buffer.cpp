// vertexbuffer.cpp - Vertex/Index Buffer package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Sep 15, 2022

#include "main/core.h"
#include "client.h"
#include "buffer.h"

// ******** vertex buffer objects ********

VertexBuffer::VertexBuffer(void *data, size_t size, int mode)
: szData(size), mode(mode)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, mode);
}

VertexBuffer::VertexBuffer(int nBuffers)
: szData(0)
{
    glGenBuffers(nBuffers, &id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &id);
}

void VertexBuffer::allocate(size_t size, void *data, int nmode)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, nmode);
    szData = size;
    mode = nmode;
}

void VertexBuffer::update(void *data, size_t size) const
{
    if (size > szData)
        size = szData;
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void *VertexBuffer::map() const
{
    void *map = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    return map;
}

void VertexBuffer::unmap() const
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ******** index buffer objects ********

IndexBuffer::IndexBuffer(indexType *data, size_t size, int mode)
: mode(mode)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(indexType), data, mode);
    count = size;
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &id);
}

void *IndexBuffer::map() const
{
    bind();
    void *map = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
    return map;
}

void IndexBuffer::unmap() const
{
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    unbind();
}

void IndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// ******** vertex array objects ********

VertexArray::VertexArray(int nBuffers)
{
    glGenVertexArrays(nBuffers, &id);

    vboList.clear();
    eboList.clear();
}

VertexArray::~VertexArray()
{
    // Clear all vertex/index buffer objects
    for(auto vbo : vboList)
        delete vbo;
    for(auto ebo : eboList)
        delete ebo;
    vboList.clear();
    eboList.clear();

    // Delete VAO id.
    glDeleteVertexArrays(1, &id);
}

void *VertexArray::create(int nBuffers, arrayType type)
{
    VertexArray *vbo = nullptr;
    switch(type)
    {
    case VBO:
        return new VertexBuffer(nBuffers);

    // case EBO:
    //     return new IndexBuffer(nBuffers);
    }
    return nullptr;
}

void VertexArray::bind() const
{
    glBindVertexArray(id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}
