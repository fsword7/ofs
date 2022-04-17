// mesh.cpp - Mesh object package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/mesh.h"

Mesh *Mesh::create(Context &ctx, int nvtx, vtxef_t *vtx, int nidx, uint16_t *idx)
{
    Mesh *mesh = new Mesh(ctx);

    mesh->nvtx = nvtx;
    mesh->vtx  = vtx;
    mesh->nidx = nidx;
    mesh->idx  = idx;

    return mesh;
}
