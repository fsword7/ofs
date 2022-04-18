// surface.h - Surface/terrain Package
//
// Author:  Tim Stark
// Date:    Apr 17, 20222

#pragma once

#include "utils/tree.h"

class Object;
class Context;
class SurfaceManager;

class SurfaceTile : public Tree<SurfaceTile, QTREE_NODES>
{
    friend class SurfaceManager;
    // friend class SurfaceHandle;

public:
    SurfaceTile(SurfaceManager &mgr, uint32_t lod, uint32_t ilat, uint32_t ilng,
        SurfaceTile *parent = nullptr);
    ~SurfaceTile();

    void render();

private:
    SurfaceManager &smgr;

    uint32_t lod, ilat, ilng;
    vec3d_t  center;

    Mesh *mesh = nullptr;

    // Texture image parameters
    bool    txOwn = false;
    tcrd_t  tcRange;
    // Texture *txImage = nullptr;

    // Elevation data parameters
    bool     elevOwn = false;
    int16_t *elev = nullptr;
    int16_t *ggelev = nullptr;
};

class SurfaceManager
{
public:
    SurfaceManager(Context &ctx, const Object &object);
    ~SurfaceManager();

    Mesh *createSphere(int lod, int ilat, int ilng, int grids, const tcrd_t &tcr);

    void render(SurfaceTile *tile);
    void render();

private:
    Context &ctx;
    const Object &object;

    SurfaceTile *tiles[2];
};