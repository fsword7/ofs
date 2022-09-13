// surface.cpp - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#include "main/core.h"
#include "scene.h"
#include "surface.h"

// ******** Surface Tile ********

SurfaceTile::SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent)
: Tree(parent), mgr(mgr), lod(lod), ilat(ilat), ilng(ilng)
{

}

SurfaceTile::~SurfaceTile()
{

}

// ******** Surface Manager ********

SurfaceManager::SurfaceManager(ObjectHandle object, Scene &scene)
: object(object), scene(scene)
{

    tiles[0] = new SurfaceTile(*this, 0, 0, 0);
    tiles[1] = new SurfaceTile(*this, 0, 0, 1);
}

SurfaceManager::~SurfaceManager()
{
    delete tiles[0],tiles[1];
}

mat4d_t SurfaceManager::getWorldMatrix(int ilat, int nlat, int ilng, int nlng)
{

    mat4d_t grot;
    mat4d_t lrot;
    mat4d_t wrot;

    double lat;
    double lng = (double(ilng) / double(nlng)) * (pi*2.0) + pi;
    double slng = sin(lng), clng = cos(lng);

    // lrot = { { clng,  0,   slng, 0   },
    //          { 0,     1.0, 0,    0   },
    //          { -slng, 0,   clng, 0   },  
    //          { 0,     0,   0,    1.0 }};

    double scale = 1.0;
    double objSize;
    double dx = objSize * cos(lng) * cos(lat);
    double dy = objSize * sin(lat);
    double dz = objSize * sin(lng) * cos(lat);

    wrot(4,1) = (dx*grot(1,1) + dy*grot(1,2) + dz*grot(1,3)) * scale;
    wrot(4,2) = (dx*grot(2,1) + dy*grot(2,2) + dz*grot(2,3)) * scale;
    wrot(4,3) = (dx*grot(3,1) + dy*grot(3,2) + dz*grot(3,3)) * scale;

    return mat4d_t().Identity();
}

void SurfaceManager::createSpherePatch(int grid, int lod, int ilat, int ilng, int16_t *elev, double selev, double gelev)
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    double mlat0  = pi * double(nlat / 2-ilat-1) / double(nlat);
    double mlat1  = pi * double(nlat / 2-ilat) / double(nlat);
    double mlng0  = 0.0;
    double mlng1  = (pi*2.0) / double(nlng);

    vec3d_t pos, nml;
    double  radius = objSize;
    
    double slat, clat, slng, clng;
    double lat, lng;
    double erad;

    // Initialize vertices
    int nvtx  = (grid+1)*(grid+1);
    int nvtxb = nvtx + grid+1 + grid+1;
    Vertex *vtx = new Vertex[nvtxb];
    int cvtx = 0;

    for (int idx0 = 0; idx0 < grid; idx0++)
    {
        lat = mlat0 + (mlat1-mlat0) * double(idx0)/double(grid);       
        slat = sin(lat), clat = cos(lat);
        for (int idx1 = 0; idx1 < grid; idx1++)
        {
            lng = mlng0 + (mlng1-mlng0) * double(idx1)/double(grid);
            slng = sin(lng), clng = cos(lng);

            erad = radius + gelev;
            if (elev != nullptr)
                erad += double(elev[(idx0+1)*ELEV_STRIDE + (idx1+1)]) * selev;
            nml = vec3d_t(clat*clng, slat, clat*slng);
            pos = nml * erad;

            vtx[cvtx].vx = float(pos.x());
            vtx[cvtx].vy = float(pos.y());
            vtx[cvtx].vz = float(pos.z());

            vtx[cvtx].nx = float(nml.x());
            vtx[cvtx].ny = float(nml.y());
            vtx[cvtx].nz = float(nml.z());

            // vtx[cvtx].tu0 = vtx[cvtx].tu0*tuRange + range->tumin;
            // vtx[cvtx].tv0 = vtx[cvtx].tv0*tvRange + range->tvmin;

            cvtx++;
        }
    }

    // Initialize indices
    int nidx = 2 * grid*grid * 3;
    uint16_t *idx = new uint16_t[nidx];
    int cidx = 0;

    if (elev != nullptr)
    {

    }
    else
    {
        for (int idx0 = 0, nofs0 = 0; idx0 < grid; idx0++)
        {
            int nofs1 = nofs0+grid+1;
            for (int idx1 = 0; idx1 < grid; idx1++)
            {
                idx[cidx++] = nofs0+idx1;
                idx[cidx++] = nofs1+idx1;
                idx[cidx++] = nofs0+idx1+1;
                idx[cidx++] = nofs1+idx1+1;
                idx[cidx++] = nofs0+idx1+1;
                idx[cidx++] = nofs1+idx1;
            }
            nofs0 = nofs1;
        }
    }

    if (elev != nullptr)
    {

    }
}