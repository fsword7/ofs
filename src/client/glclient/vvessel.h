// vvessel.h - Visual Vessel package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#pragma once

class vMesh;
class Vessel;

struct MeshEntry
{
    vMesh *mesh = nullptr;
    glm::mat4 trans;
    bool isVisible;
};
class vVessel : public vObject
{
public:
    vVessel(const Object *object, Scene &scene);
    ~vVessel();

    void loadMeshes();

    void render(const ObjectListEntry &ole) override;

private:
    const Vessel *vessel = nullptr;
    std::vector<MeshEntry> meshList;

};