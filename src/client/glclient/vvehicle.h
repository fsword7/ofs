// vvehicle.h - Visual Vehicle package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#pragma once

class vMesh;
class Vehicle;
struct MeshGroup;
struct anim_t;
struct ancomp_t;

struct vMeshEntry
{
    vMesh *mesh = nullptr;
    glm::mat4 trans;
    bool isVisible;
};
class vVehicle : public vObject
{
public:
    vVehicle(const Object *object, Scene &scene);
    ~vVehicle();

    void loadMeshes();
    void initAnimations();
    void clearMeshes();
    void clearAnimations();

    void updateAnimations();

    void animateComponent(ancomp_t *ac, const glm::mat4 &T);
    void animate(anim_t *an, double state, int midx);
    void update(int now);
    void render(const ObjectListEntry &ole) override;

private:
    const Vehicle *vehicle = nullptr;
    std::vector<vMeshEntry> meshList;

    std::vector<double> animpState;
};