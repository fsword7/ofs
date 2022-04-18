// mesh.h - Mesh object package for OpenGL package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#pragma once

template <typename T>
struct Vertex32e
{
    T vx, vy, vz;   // vertex
    T ex, ey, ez;   // vertex extension
    T nx, ny, nz;   // normal for lightings
    T tu, tv;       // texture coordinates
};

typedef Vertex32e<float> vtxef_t;

class Context;

class Mesh
{
public:
    Mesh(Context &ctx) : ctx(ctx)
    { }
    ~Mesh() = default;

    static Mesh *create(Context &gl, int nvtx, vtxef_t *vtx, int nidx, uint16_t *idx);

    void render();

private:
    Context &ctx;

    // Object parameters
    int       nvtx = 0, nidx = 0;
    vtxef_t  *vtx = nullptr;
    uint16_t *idx = nullptr;
};