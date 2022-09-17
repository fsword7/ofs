// surface.h - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#pragma once

#include "utils/tree.h"

#define ELEV_STRIDE 0

class VertexArray;
class VertexBuffer;
class IndexBuffer;

struct Vertex
{
    float vx, vy, vz;   // Vertex position
    float nx, ny, nz;   // Normal position
    float tu, tv;       // Texture coordinates
};

struct Mesh
{
    Mesh(int nvtx, Vertex *vtx, int nidx, uint16_t *idx)
    : nvtx(nvtx), vtx(vtx), nidx(nidx), idx(idx)
    { }

    ~Mesh()
    {
        delete [] vtx;
        delete [] idx;
    }

    void upload();

    VertexArray    *vao = nullptr;;
    VertexBuffer   *vbo = nullptr;
    IndexBuffer    *ibo = nullptr;

    int       nvtx;
    Vertex   *vtx;
    int       nidx;
    uint16_t *idx;
};

struct tcRange
{
    double tumin, tumax;
    double tvmin, tvmax;
};

class Scene;
class SurfaceManager;

class SurfaceTile : public Tree<SurfaceTile, QTREE_NODES>
{
public:
    SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent = nullptr);
    ~SurfaceTile();

    void load();
    void render();

private:
    SurfaceManager &mgr;

    int lod;
    int ilat, ilng;

    Mesh *mesh = nullptr;
};

class SurfaceManager
{
    friend class SurfaceTile;

public:
    SurfaceManager(ObjectHandle object, Scene &scene);
    ~SurfaceManager();

    glm::dmat4 getWorldMatrix(int ilat, int nlat, int ilng, int nlng);

    Mesh *createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
        int16_t *elev = nullptr, double selev = 1.0, double gelev = 0.0);

private:
    ObjectHandle object;
    Scene &scene;

    SurfaceTile *tiles[2];

    double objSize = 0.0;
};