// surface.cpp - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#include "main/core.h"
#include "client.h"
#include "scene.h"
#include "buffer.h"
#include "surface.h"

// ******** Surface Tile ********

static tcRange range = { 0, 1, 0, 1 };

SurfaceTile::SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent)
: Tree(parent), mgr(mgr), lod(lod), ilat(ilat), ilng(ilng)
{

}

SurfaceTile::~SurfaceTile()
{
    if (mesh != nullptr)
        delete mesh;
}

void SurfaceTile::load()
{

    mesh = mgr.createSpherePatch(32, lod, ilat, ilng, range);
}

void SurfaceTile::render()
{
    if (mesh == nullptr)
        return;
    if (mesh->vao == nullptr)
        mesh->upload();

    mesh->vao->bind();

    glDrawElements(GL_TRIANGLES, mesh->ibo->getCount(), GL_UNSIGNED_SHORT, 0);
    mesh->vao->unbind();
}

// ******** Surface Manager ********

SurfaceManager::SurfaceManager(ObjectHandle object, Scene &scene)
: object(object), scene(scene)
{
    objSize = ofsGetObjectRadius(object);

    for (int idx = 0; idx < 2; idx++)
    {
        tiles[idx] = new SurfaceTile(*this, 0, 0, idx);
        tiles[idx]->load();
    }
}

SurfaceManager::~SurfaceManager()
{
    delete tiles[0],tiles[1];
}

glm::dmat4 SurfaceManager::getWorldMatrix(int ilat, int nlat, int ilng, int nlng)
{

    glm::dmat4 grot;
    glm::dmat4 lrot;
    glm::dmat4 wrot;

    double lat;
    double lng = (double(ilng) / double(nlng)) * (pi*2.0) + pi;
    double slng = sin(lng), clng = cos(lng);

    lrot = { { clng,  0,   slng, 0   },
             { 0,     1.0, 0,    0   },
             { -slng, 0,   clng, 0   },  
             { 0,     0,   0,    1.0 }};

    double scale = 1.0;
    double dx = objSize * cos(lng) * cos(lat);
    double dy = objSize * sin(lat);
    double dz = objSize * sin(lng) * cos(lat);

    // Calculate translation with per-tile model matrix
    wrot[3][0] = (dx*grot[0][0] + dy*grot[0][1] + dz*grot[0][2] /* + prm.cpos */) * scale;
    wrot[3][1] = (dx*grot[1][0] + dy*grot[1][1] + dz*grot[1][2] /* + prm.cpos */) * scale;
    wrot[3][2] = (dx*grot[2][0] + dy*grot[2][1] + dz*grot[2][2] /* + prm.cpos */) * scale;

    return wrot;
}

Mesh *SurfaceManager::createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
    int16_t *elev, double selev, double gelev)
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    double mlat0  = pi * double(nlat / 2-ilat-1) / double(nlat);
    double mlat1  = pi * double(nlat / 2-ilat) / double(nlat);
    double mlng0  = 0.0;
    double mlng1  = (pi*2.0) / double(nlng);

    glm::dvec3 pos, nml;
    double  radius = objSize;
    
    double slat, clat, slng, clng;
    double lat, lng;
    double erad;

    // Initialize vertices
    int nvtx  = (grid+1)*(grid+1);
    int nvtxb = nvtx + grid+1 + grid+1;
    Vertex *vtx = new Vertex[nvtxb];
    int cvtx = 0;

    float tur = range.tumax - range.tumin;
    float tvr = range.tvmax - range.tvmin;
    float tu, tv;

    for (int y = 0; y < grid; y++)
    {
        lat = mlat0 + (mlat1-mlat0) * double(y)/double(grid);       
        slat = sin(lat), clat = cos(lat);
        tu = range.tumin + tur * float(y)/float(grid);

        for (int x = 0; x < grid; x++)
        {
            lng = mlng0 + (mlng1-mlng0) * double(x)/double(grid);
            slng = sin(lng), clng = cos(lng);
            tv = range.tvmin + tvr + float(x)/float(grid);
            erad = radius + gelev;

            if (elev != nullptr)
                erad += double(elev[(y+1)*ELEV_STRIDE + (x+1)]) * selev;
            nml = glm::dvec3(clat*clng, slat, clat*slng);
            pos = nml * erad;

            vtx[cvtx].vx = float(pos.x);
            vtx[cvtx].vy = float(pos.y);
            vtx[cvtx].vz = float(pos.z);

            vtx[cvtx].nx = float(nml.x);
            vtx[cvtx].ny = float(nml.y);
            vtx[cvtx].nz = float(nml.z);

            vtx[cvtx].tu = tu;
            vtx[cvtx].tv = tv;
   
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
        for (int y = 0, nofs0 = 0; y < grid; y++)
        {
            int nofs1 = nofs0+grid+1;
            for (int x = 0; x < grid; x++)
            {
                idx[cidx++] = nofs0+x;
                idx[cidx++] = nofs1+x;
                idx[cidx++] = nofs0+x+1;
                idx[cidx++] = nofs1+x+1;
                idx[cidx++] = nofs0+x+1;
                idx[cidx++] = nofs1+x;
            }
            nofs0 = nofs1;
        }
    }

    if (elev != nullptr)
    {

    }

    return new Mesh(nvtx, vtx, nidx, idx);
}

// ******** Mesh ********

void Mesh::upload()
{
    vao = new VertexArray();
    vao->bind();

    vbo = new VertexBuffer(vtx, nvtx * sizeof(Vertex));
    vbo->bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)12);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)24);
    glEnableVertexAttribArray(2);

    ibo = new IndexBuffer(idx, nidx);
    ibo->bind();

    vao->unbind();
}