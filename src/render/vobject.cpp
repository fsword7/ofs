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
    case ObjectType::objCelestialStar:
        return new vStar(obj, scene);
    case ObjectType::objCelestialBody:
        return new vBody(obj, scene);
    case ObjectType::objVessel:
        return new vVessel(obj, scene);
    }

    return nullptr;
}

void vObject::render(renderParam &prm, ObjectProperties &op)
{
}

void vObject::render(renderParam &prm,
    ObjectProperties &op, LightState &lights)
{
}

// ******** Scene ********

vObject *Scene::addVisualObject(const Object &object)
{
    vObject *vobj = nullptr;

    Logger::getLogger()->verbose("Adding visual object: {}\n",
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