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

void Mesh::render()
{

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDrawElements(GL_TRIANGLES, nidx, GL_UNSIGNED_SHORT, 0);

    glDisable(GL_CULL_FACE);
}