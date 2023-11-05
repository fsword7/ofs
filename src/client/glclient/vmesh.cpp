// vmesh.cpp - Visual Mesh Package for Vessels and others.
//
// Author:  Tim Stark
// Date:    Nov 4, 2023

#include "main/core.h"

#include "client.h"
#include "vmesh.h"

vMesh::~vMesh()
{
    deleteGroups();
}

int vMesh::addGroup(GroupMesh *group, bool deepCopy)
{
    // if (deepCopy)
    //     copyGroup(group);
    // else
        groups.push_back(group);

    return 0;
}

// void vMesh::copyGroup(GroupMesh *group)
// {
    
// }|

void vMesh::deleteGroup(GroupMesh *group)
{
    if (group->vtx != nullptr)
        delete [] group->vtx;
    if (group->idx != nullptr)
        delete [] group->idx;
    delete group;
}

void vMesh::deleteGroups()
{
    for (auto group : groups)
        deleteGroup(group);
    groups.clear();
}

void vMesh::renderGroup(const GroupMesh &group)
{
    if (group.nvtx == 0 || group.nidx == 0)
        return;
    group.vao.bind();
    glDrawElements(GL_TRIANGLES, group.ibo.getCount(), GL_UNSIGNED_SHORT, 0);
    group.vao.unbind();
}

void vMesh::render()
{

    bool bSkipped = false;

    for (auto group : groups)
    {
        if (group->userFlag & 2)
        {
            bSkipped = true;
            continue;
        }

        renderGroup(*group);

        bSkipped = false;
    }
}