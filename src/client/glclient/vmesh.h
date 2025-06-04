// vmesh.h - Visual mesh package for vehicles and others
//
// Author:  Tim Stark
// Date:    Nov 13, 2024

#pragma once

#include "buffer.h"

class Mesh;
struct MeshGroup;

struct vMeshVertex
{
    // float vx, vy, vz;   // Vertices
    // float nx, ny, nz;   // Normals
    // float tx, ty, tz;   // Tangets
    // float tu, tv;       // Texture Coordinates

    glm::vec3 vtx;
    glm::vec3 nml;
    glm::vec3 tgt;
    glm::vec2 tc;
};

using mvtx_t = vMeshVertex;

struct vMeshGroup
{
    std::vector<mvtx_t> vertices;
    std::vector<uint16_t> indices;

    VertexArray  vao;
    VertexBuffer vbo;
    IndexBuffer  ibo;
};

class vMesh
{
public:
    vMesh(Mesh *mesh);
    ~vMesh();

    void copyGroup(vMeshGroup *glgrp, MeshGroup *mgrp);

    void computeTangets();

    void renderGroup(const vMeshGroup &grp);
    void render(const glm::mat4 &model);

private:
    std::vector<vMeshGroup *> groups;

};

class vMeshManager
{
public:

private:
    std::vector<vMesh *> meshList;
};