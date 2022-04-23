// surface.h - Surface/terrain Package
//
// Author:  Tim Stark
// Date:    Apr 17, 20222

#pragma once

#include "render/ztreemgr.h"
#include "osd/gl/context.h"
#include "osd/gl/shader.h"
// #include "osd/gl/texture.h"
#include "utils/tree.h"

#define TILE_VALID      0x8000'0000
#define TILE_ACTIVE     0x4000'0000

class Object;
class Context;
class SurfaceManager;
class Texture;

class SurfaceTile : public Tree<SurfaceTile, QTREE_NODES>
{
    friend class SurfaceManager;
    // friend class SurfaceHandle;

public:
    enum TileState
    {
        Invalid      = 0x0000,
        InQueue      = 0x0001,
        Loading      = 0x0002,
        Initializing = 0x0004,
        NoImage      = 0x0008,
        Inactive     = TILE_VALID,
        Active       = TILE_VALID|TILE_ACTIVE,
        Invisible    = TILE_VALID|TILE_ACTIVE|0x0100,
        Rendering    = TILE_VALID|TILE_ACTIVE|0x0200
    };

    SurfaceTile(SurfaceManager &mgr, uint32_t lod, uint32_t ilat, uint32_t ilng,
        SurfaceTile *parent = nullptr);
    ~SurfaceTile();

    vec3d_t setCenter();
    void setSubtextureRange(const tcrd_t &ptcr);

    void load();
    void render(renderParam &prm);

    inline Texture *getTexture() const { return txImage; }

private:
    SurfaceManager &smgr;

    TileState state = Invalid;
    uint32_t  lod, ilat, ilng;
    vec3d_t   center;

    Mesh *mesh = nullptr;

    // Texture image parameters
    bool    txOwn = false;
    tcrd_t  tcRange;
    Texture *txImage = nullptr;

    // Elevation data parameters
    bool     elevOwn = false;
    int16_t *elev = nullptr;
    int16_t *ggelev = nullptr;
};

class SurfaceManager
{
    friend class SurfaceTile;

public:
    SurfaceManager(Context &ctx, const Object &object);
    ~SurfaceManager();

    Mesh *createSphere(int lod, int ilat, int ilng, int grids, const tcrd_t &tcr);

    void update(SurfaceTile *tile, renderParam &prm);
    void render(SurfaceTile *tile, renderParam &prm);
    void render(renderParam &prm, ObjectProperties &op);

private:
    Context &ctx;
    const Object &object;

    fs::path surfaceFolder;

    ShaderProgram *pgmPlanet = nullptr;
    mat4Uniform mvp;

    zTreeManager *zTrees[5];
    SurfaceTile *tiles[2];
};