// vertexbuffer.cpp - Vertex/Index Buffer package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Sep 15, 2022

#include "main/core.h"
#include "glclient.h"
#include "buffer.h"

// ******** vertex array objects ********

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
}

void VertexArray::bind() const
{
    glBindVertexArray(id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

// ******** vertex buffer objects ********

VertexBuffer::VertexBuffer(void *data, size_t size)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &id);
}

void VertexBuffer::update(void *data, size_t size) const
{
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void *VertexBuffer::map() const
{
    bind();
    void *map = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    return map;
}

void VertexBuffer::unmap() const
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
    unbind();
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

IndexBuffer::IndexBuffer(indexType *data, size_t size)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(indexType), data, GL_STATIC_DRAW);
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
