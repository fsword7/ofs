// vmesh.h - Visual Mesh Package for Vessels and others.
//
// Author:  Tim Stark
// Date:    Nov 4, 2023

#pragma once

#include "buffer.h"

struct vVertexMesh
{
    float x, y, z;      // XYZ position
    float nx, ny, nz;   // XYZ normal
    float tx, ty, tz;   // XYZ tangent
    float tu, tv;       // UV texture coordinate
};

struct vGroupMesh
{
    int nvtx;
    int nidx;

    vVertexMesh *vtx = nullptr;
    uint16_t  *idx = nullptr;

    uint32_t userFlag;

    VertexArray vao;
    VertexBuffer vbo;
    IndexBuffer ibo;
};

class vMesh
{
public:
    vMesh() = default;
    ~vMesh();

    int addGroup(vGroupMesh *group, bool deepCopy = true);
    // void copyGroup(GroupMesh *group);
    void deleteGroup(vGroupMesh *group);
    void deleteGroups();

    vGroupMesh *getGroup(int idx)        { return idx < groups.size() ? groups[idx] : nullptr; }

    void renderGroup(const vGroupMesh &group);
    void render();

private:
    std::vector<vGroupMesh *> groups;

};