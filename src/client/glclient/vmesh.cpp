// vmesh.cpp - Visual mesh package for vehicles and others
//
// Author:  Tim Stark
// Date:    Nov 13, 2024

#include "main/core.h"
#include "engine/mesh.h"
#include "client.h"
#include "renderer.h"
#include "vmesh.h"


vMesh::vMesh(Mesh *mesh)
{

    int ngrps = mesh->getGroupSize();
    groups.reserve(ngrps);
    for (int idx = 0; idx < ngrps; idx++)
        copyGroup(groups[idx], mesh->getGroup(idx));

}

vMesh::~vMesh()
{

}

void vMesh::copyGroup(vMeshGroup *dst, MeshGroup *src)
{

    // dstv->vertices.reserve(src->nvtx);
    for (int vidx = 0; vidx < src->nvtx; vidx++)
    {
        MeshVertex  &srcv = src->vtx[vidx];
        vMeshVertex &dstv = dst->vertices[vidx];

        dstv.vtx = srcv.vtx;
        dstv.nml = srcv.nml;
        dstv.tc  = srcv.tc;
    }

    dst->indices.reserve(src->nidx);
    for (int iidx = 0; iidx < src->nidx; iidx++)
        dst->indices[iidx] = src->idx[iidx];

}

void vMesh::computeTangets()
{
    
}

void vMesh::renderGroup(const vMeshGroup &grp)
{
    if (grp.ibo.getCount() == 0 || grp.vbo.getCount() == 0)
        return;
    grp.vao.bind();
    glDrawElements(GL_TRIANGLES, grp.ibo.getCount(), GL_UNSIGNED_SHORT, 0);
    grp.vao.unbind();
}

void vMesh::render(const glm::mat4 &model)
{

    for (auto &grp : groups) {

        renderGroup(*grp);
    }
}