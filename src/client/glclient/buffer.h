// vertexbuffer.h - Vertex/Index Buffer package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Sep 15, 2022

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

private:
    GLuint id;
};

class VertexBuffer
{
public:
    VertexBuffer(void *data, size_t size);
    ~VertexBuffer();

    void update(void *data, size_t size) const;
    void *map() const;
    void unmap() const;

    void bind() const;
    void unbind() const;

private:
    GLuint id;
};

class IndexBuffer
{
    using indexType = uint16_t;

public:
    IndexBuffer(indexType *data, size_t size);
    ~IndexBuffer();

    inline uint32_t getCount() const { return count; }

    void *map() const;
    void unmap() const;

    void bind() const;
    void unbind() const;

private:
    GLuint id;
    uint32_t count;
};