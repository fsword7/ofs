// vvehicle.cpp - Visual Vehicle package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/vehicle/vehicle.h"
#include "engine/mesh.h"

#include "client.h"
#include "scene.h"
#include "vmesh.h"
#include "vobject.h"
#include "vvehicle.h"

vVehicle::vVehicle(const Object *object, Scene &scene)
: vObject(object, scene)
{
    vehicle = dynamic_cast<const Vehicle *>(object);
    MeshGroup *mesh = nullptr; // vehicle->getMesh();
    loadMeshes();
    initAnimations();
}

vVehicle::~vVehicle()
{
    clearAnimations();
    clearMeshes();
}

void vVehicle::loadMeshes()
{

}

void vVehicle::clearMeshes()
{
    for (auto &entry : meshList)
        if (entry.mesh != nullptr)
            delete entry.mesh;
    meshList.clear();
}

void vVehicle::initAnimations()
{
    canlist_t &animList = vehicle->getAnimationList();
    animpState.clear();
    animpState.reserve(animList.size());
    for (int idx = 0; idx < animList.size(); idx++)
        animpState[idx] = animList[idx]->state;
}

void vVehicle::clearAnimations()
{

}

void vVehicle::updateAnimations()
{
    canlist_t &animList = vehicle->getAnimationList();
    double newState;

    for (int idx = 0; idx < animList.size(); idx++)
    {
        anim_t *entry = animList[idx];
        if (entry->compList.empty())
            continue;
        if (animpState[idx] != (newState = entry->state))
            animate(entry, newState, 0), animpState[idx] = newState;
    }
}

void vVehicle::animateComponent(ancomp_t *ac, const glm::mat4 &T)
{

}

void vVehicle::animate(anim_t *an, double state, int midx)
{
    glm::mat4 T;

    for (auto comp : an->compList)
    {
        switch (comp->trans.type)
        {
        case antrans_t::null:
            T = glm::mat4(1.0f);
            animateComponent(comp, T);
            break;
        case antrans_t::rotate:
            animateComponent(comp, T);
            break;
        case antrans_t::translate:
            animateComponent(comp, T);
            break;
        case antrans_t::scale:
            animateComponent(comp, T);
            break;
        }
    }
}

void vVehicle::update(int now)
{
    vObject::update(now);
    // updateAnimate(now);
}

void vVehicle::render(const ObjectListEntry &ole)
{
    for (auto mesh : meshList)
    {
        if (mesh.isVisible == false)
            continue;
        
        // Render external mesh
        // mesh.render();

        // Render HUD/MFD panels
    }
}