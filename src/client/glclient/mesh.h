// mesh.h - Mesh Package
//
// Author:  Tim Stark
// Date:    Nov 13, 2024

#pragma once

class Mesh;

struct glMeshVertex
{
    float vx, vy, vz;   // Vertices
    float nx, ny, nz;   // Normals
    float tx, ty, tz;   // Tangets
    float tu, tv;       // Texture Coordinates
};

using mvtx_t = glMeshVertex;

struct glMeshGroup
{
    std::vector<mvtx_t *> vertices;
};

class glMesh
{
public:
    glMesh(Mesh *mesh);
    ~glMesh();

    void computeTangets();

private:

};

class glMeshManager
{
public:

private:
    std::vector<glMesh *> meshList;
};