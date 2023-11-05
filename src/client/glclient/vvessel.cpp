// vvessel.cpp - Visual Vessel package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/vessel.h"

#include "client.h"
#include "scene.h"
#include "vmesh.h"
#include "vobject.h"
#include "vvessel.h"

vVessel::vVessel(const Object *object, Scene &scene)
: vObject(object, scene)
{
    vessel = dynamic_cast<const Vessel *>(object);
}

vVessel::~vVessel()
{
    
}

void vVessel::loadMeshes()
{

}

void vVessel::render(const ObjectListEntry &ole)
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