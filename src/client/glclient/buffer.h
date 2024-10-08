// vertexbuffer.h - Vertex/Index Buffer package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Sep 15, 2022

#pragma once

class VertexBuffer
{
public:
    VertexBuffer(void *data, size_t size, int mode = GL_DYNAMIC_DRAW);
    VertexBuffer(int nBuffers);
    ~VertexBuffer();

    void allocate(size_t size, void *data = nullptr, int mode = GL_DYNAMIC_DRAW);
    void update(void *data, size_t size) const;
    void *map() const;
    void unmap() const;

    inline uint32_t getCount() const { return count; }

    void bind() const;
    void unbind() const;

private:
    GLuint id;
    int mode;
    uint32_t count = 0;
    size_t szData = 0;
};

class IndexBuffer
{
    using indexType = uint16_t;

public:
    IndexBuffer(indexType *data, size_t size, int mode = GL_STATIC_DRAW);
    IndexBuffer(int nBuffers);
    ~IndexBuffer();

    inline uint32_t getCount() const { return count; }

    void *map() const;
    void unmap() const;

    void bind() const;
    void unbind() const;

private:
    GLuint id;
    int mode;
    uint32_t count = 0;
    size_t szData = 0;
};

class VertexArray
{
public:
    enum arrayType { VBO, IBO };

    VertexArray(int nBuffers = 1);
    ~VertexArray();

    void addVertices(VertexBuffer *vbo) { vboList.push_back(vbo); }
    void addIndices(IndexBuffer *ibo)   { iboList.push_back(ibo); }

    void *create(int nBuffers, arrayType type);

    void bind() const;
    void unbind() const;

private:
    GLuint id;

    std::vector<VertexBuffer *> vboList;
    std::vector<IndexBuffer *> iboList;
};
