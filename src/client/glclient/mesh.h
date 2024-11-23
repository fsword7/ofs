// mesh.h - Mesh Package
//
// Author:  Tim Stark
// Date:    Nov 13, 2024

#pragma once

#include "buffer.h"

class Mesh;

struct glMeshVertex
{
    float vx, vy, vz;   // Vertices
    float nx, ny, nz;   // Normals
    float tx, ty, tz;   // Tangets
    float tu, tv;       // Texture Coordinates

    // glm::vec3 vtx;
    // glm::vec3 nml;
    // glm::vec3 tgt;
    // glm::vec3 tc;
};

using mvtx_t = glMeshVertex;

struct glMeshGroup
{
    std::vector<mvtx_t *> vertices;

    VertexArray  vao;
    VertexBuffer vbo;
    IndexBuffer  ibo;
};

class glMesh
{
public:
    glMesh(Mesh *mesh);
    ~glMesh();

    void copyGroup(glMeshGroup *glgrp, MeshGroup *mgrp);

    void computeTangets();

    void renderGroup(const glMeshGroup &grp);
    void render(const glm::mat4 &model);

private:
    std::vector<glMeshGroup *> groups;

};

class glMeshManager
{
public:

private:
    std::vector<glMesh *> meshList;
};