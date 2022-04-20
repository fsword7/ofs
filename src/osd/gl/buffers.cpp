// buffers.cpp - Buffer package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Apr 20, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"

VertexBuffer::VertexBuffer(const Context &gl, int nArrays)
: gl(gl)
{
    glGenVertexArrays(nArrays, &vao);
}

VertexBuffer::~VertexBuffer()
{
    if (vbo > 0)
        glDeleteBuffers(1, &vbo);
    if (ebo > 0)
        glDeleteBuffers(1, &ebo);
    if (vao > 0)
        glDeleteVertexArrays(1, &vao);
}

GLuint VertexBuffer::createBuffer(BufferType type, GLuint nBuffers)
{
    bind();
    switch(type)
    {
    case VBO:
        glGenBuffers(nBuffers, &vbo);
        return vbo;
    case EBO:
        glGenBuffers(nBuffers, &ebo);
        return ebo;
    case VAO:
    default:
        break;
    }
    return 0;
}

void VertexBuffer::assign(BufferType type, void *data, GLuint size)
{
    bind();
    switch(type)
    {
    case VBO:
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
        break;
    case EBO:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
        break;
    case VAO:
    default:
        break;
    }
}