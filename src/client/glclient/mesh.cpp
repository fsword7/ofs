// mesh.cpp - Mesh Package
//
// Author:  Tim Stark
// Date:    Nov 13, 2024

#include "main/core.h"
#include "engine/mesh.h"
#include "client.h"
#include "renderer.h"
#include "mesh.h"


glMesh::glMesh(Mesh *mesh)
{

    int ngrps = mesh->getGroupSize();
    groups.reserve(ngrps);
    for (int idx = 0; idx < ngrps; idx++)
        copyGroup(groups[idx], mesh->getGroup(idx));

}

glMesh::~glMesh()
{

}

void glMesh::copyGroup(glMeshGroup *dst, MeshGroup *src)
{
    
}

void glMesh::computeTangets()
{
    
}

void glMesh::renderGroup(const glMeshGroup &grp)
{
    if (grp.ibo.getCount() == 0 || grp.vbo.getCount() == 0)
        return;
    grp.vao.bind();
    glDrawElements(GL_TRIANGLES, grp.ibo.getCount(), GL_UNSIGNED_SHORT, 0);
    grp.vao.unbind();
}

void glMesh::render(const glm::mat4 &model)
{

}