// mesh.h - Mesh Package for spacecrafts
//
// Author:  Tim Stark
// Date:    Nov 5, 2023

#pragma once

struct MeshVertex
{
    glm::vec3 vtx;
    glm::vec3 nml;
    glm::vec2 tc;
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

class Texture;

class Mesh
{
    friend std::istream &operator >> (std::istream &is, Mesh &mesh);

public:
    Mesh() = default;
    ~Mesh();

    void setup();
    void clear();

    void addGroup(MeshGroup *group);
    void addMaterial(MeshMaterial *mtrl);

    inline int getGroupSize() const             { return groups.size(); }
    inline MeshGroup *getGroup(int idx)         { return idx < groups.size() ? groups[idx] : nullptr; }

    void calculateNormals(MeshGroup *group, bool missing);

    void load(json &config, Mesh &mesh);

    friend std::istream &operator >> (std::istream &is, Mesh &mesh);

private:
    std::vector<MeshGroup *> groups;
    std::vector<MeshMaterial *> materials;
    std::vector<Texture *> txImages;
};

class MeshManager
{
public:
    MeshManager() = default;
    ~MeshManager();

    void cleanup();

    const Mesh *loadMesh(cstr_t &fname);

private:
    std::vector<Mesh *> meshList;
};