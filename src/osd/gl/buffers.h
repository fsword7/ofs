// buffers.h - Buffer package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Apr 20, 2022

#pragma once

class Context;

class VertexBuffer
{
public:
    enum BufferType { VAO, VBO, EBO };

    VertexBuffer(const Context &gl, int nArrays = 1);
    ~VertexBuffer();

    inline void bind() const    { glBindVertexArray(vao); }
    inline void release() const { glBindVertexArray(0); }

    inline GLuint getVBO() const { return vbo; }
    inline GLuint getEBO() const { return ebo; }

    GLuint createBuffer(BufferType type, GLuint nBuffers);
    void assign(BufferType type, void *data, GLuint size);

private:
    const Context &gl;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0; 
};
