// mesh.cpp - Mesh object package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/texture.h"
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

void Mesh::allocate()
{
    vbuf = new VertexBuffer(ctx, 1);

    vbuf->createBuffer(VertexBuffer::VBO, 1);
    vbuf->createBuffer(VertexBuffer::EBO, 1);

    bAllocated = true;
}

void Mesh::render()
{
    if (bAllocated == false)
        allocate();
    
    vbuf->bind();
    vbuf->assign(VertexBuffer::VBO, vtx, nvtx*sizeof(vtxef_t));
    vbuf->assign(VertexBuffer::EBO, idx, nidx*sizeof(uint16_t));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vtxef_t), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vtxef_t), (void *)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vtxef_t), (void *)(6 * sizeof(GLfloat)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vtxef_t), (void *)(9 * sizeof(GLfloat)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    if (txImage != nullptr)
    {
        if (txImage->isLoaded() == false)
            txImage->load();
        glActiveTexture(GL_TEXTURE0);
        txImage->bind();
    }
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDrawElements(GL_TRIANGLES, nidx, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    glDisable(GL_CULL_FACE);
}