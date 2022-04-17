// surface.h - Surface/terrain Package
//
// Author:  Tim Stark
// Date:    Apr 17, 20222

#pragma once

class Object;

class SurfaceTile
{
public:
    SurfaceTile() = default;
    ~SurfaceTile() = default;
};

class SurfaceManager
{
public:
    SurfaceManager(Context &ctx, Object &object) : ctx(ctx), object(object)
    { }
    ~SurfaceManager() = default;

    Mesh *createSphere(int lod, int ilat, int ilng, int grids, const tcrd_t &tcr);

private:
    Context &ctx;
    Object &object;
};