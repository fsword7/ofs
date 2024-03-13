// vvehicle.cpp - Visual Vehicle package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/vehicle.h"

#include "client.h"
#include "scene.h"
#include "vmesh.h"
#include "vobject.h"
#include "vVehicle.h"

vVehicle::vVehicle(const Object *object, Scene &scene)
: vObject(object, scene)
{
    vehicle = dynamic_cast<const Vehicle *>(object);
}

vVehicle::~vVehicle()
{
    
}

void vVehicle::loadMeshes()
{

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