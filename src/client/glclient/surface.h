// surface.h - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#pragma once

#include "utils/tree.h"

#define ELEV_STRIDE 0

struct Vertex
{
    float vx, vy, vz;   // Vertex position
    float nx, ny, nz;   // Normal position
    float tu0, tu1;     // Texture coordinates
    float tv0, tv1;
};

class Scene;
class SurfaceManager;

class SurfaceTile : public Tree<SurfaceTile, QTREE_NODES>
{
public:
    SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent = nullptr);
    ~SurfaceTile();

private:
    SurfaceManager &mgr;

    int lod;
    int ilat, ilng;

};

class SurfaceManager
{
    friend class SurfaceTile;

public:
    SurfaceManager(ObjectHandle object, Scene &scene);
    ~SurfaceManager();

    mat4d_t getWorldMatrix(int ilat, int nlat, int ilng, int nlng);

    void createSpherePatch(int grid, int lod, int ilat, int ilng,
        int16_t *elev = nullptr, double selev = 1.0, double gelev = 0.0);

private:
    ObjectHandle object;
    Scene &scene;

    SurfaceTile *tiles[2];

    double objSize = 0.0;
};