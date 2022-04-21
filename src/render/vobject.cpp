// vobject.cpp - Visual Object Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#include "main/core.h"
#include "osd/gl/mesh.h"
#include "engine/object.h"
#include "render/scene.h"
#include "render/vobject.h"
#include "render/vbody.h"
#include "render/vstar.h"
#include "render/vvessel.h"

// ******** Visual Object ********

vObject *vObject::create(const Object &obj, Scene &scene)
{
    switch (obj.getType())
    {
    case Object::objCelestialStar:
        return new vStar(obj, scene);
    case Object::objCelestialBody:
        return new vBody(obj, scene);
    case Object::objVessel:
        return new vVessel(obj, scene);
    }

    return nullptr;
}

// ******** Scene ********

vObject *Scene::addVisualObject(const Object &object)
{
    vObject *vobj = nullptr;

    fmt::printf("Adding visual object: %s\n",
        object.getsName());
    vobj = vObject::create(object, *this);
    vObjectList.push_back(vobj);

    return vobj;
}

vObject *Scene::getVisualObject(const Object &object, bool bCreate)
{
    for (int idx = 0; idx < vObjectList.size(); idx++)
        if (vObjectList[idx]->getObject() == &object)
            return vObjectList[idx];

    if (bCreate == true)
        return addVisualObject(object);
    return nullptr;
}