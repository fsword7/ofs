// mesh.h - 3D mesh package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#pragma once

struct Material
{
    char        *name;
    vec4f_t     diffuse;
    vec4f_t     ambient;
    vec4f_t     specular;
    vec4f_t     emissive;
    float       power;
};

struct MeshGroup
{
    int32_t     mIndex;     // material index
    int32_t     tIndex;     // texture index
    int32_t     nvtx;       // Number of vertices
    int32_t     nidx;       // Number of Indices
    vtxf_t      *vtx;       // Vertices data
    uint32_t    *idx;       // Indices data

    uint32_t    uFlags;
    uint8_t     flags;
    int16_t     zBias;
};

class Mesh3D
{
public:
    Mesh3D() = default;
    ~Mesh3D() = default;

    void setup();
    void clear();

    MeshGroup *addGroup(vtxf_t *vtx, int nvtx, uint32_t *idx, int nidx, int midx, int tidx, int zbias);

private:
    int nGroups;        // Number of Mesh 3D groups
    int nMaterials;     // Number of materials
    int nTextures;      // Number of textures

    std::vector<MeshGroup *> groups;
    std::vector<Material *> materials;
};