// mesh.h - Mesh Package for spacecrafts
//
// Author:  Tim Stark
// Date:    Nov 5, 2023

#pragma once

struct MeshVertex
{
    double x, y, z;
    double nx, ny, nz;
    double tu, tv;
};

struct MeshGroup
{
    int nvtx, nidx;
    MeshVertex *vtx;
    uint16_t *idx;

    uint32_t Flags;
    uint32_t userFlags;
    int texIndex;
    int mtrlIndex;
};

struct MeshMaterial
{
    std::string name;
    glm::vec4   diffuse;
    glm::vec4   ambient;
    glm::vec4   specular;
    glm::vec4   emissive;
    float       power;
};

class Mesh
{
public:
    Mesh() = default;
    ~Mesh();

    void setup();
    void clear();

    void addGroup(MeshGroup *group);

    void calculateNormals(MeshGroup *group, bool missing);

    friend std::istream &operator >> (std::istream &is, Mesh &mesh);

private:
    std::vector<MeshGroup *> groups;
    std::vector<MeshMaterial *> materials;

};