// vobject.cpp - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#include "main/core.h"
#include "api/ofsapi.h"
#include "engine/object.h"
#include "client.h"
#include "scene.h"
#include "vobject.h"
#include "vbody.h"
#include "vstar.h"
#include "vvessel.h"


vObject *vObject::create(const Object *object, Scene &scene)
{
    switch (object->getType())
    {
    case ObjectType::objCelestialStar:
        return new vStar(object, scene);
    case ObjectType::objCelestialBody:
        return new vBody(object, scene);
    case ObjectType::objVessel:
        return new vVessel(object, scene);
    }

    return nullptr;
}

void vObject::update(int now)
{

    // gpos  = object->getuPosition(now);
    gpos  = object->getoPosition();
    vpos  = gpos - scene.getCamera()->getGlobalPosition();
    vdist = glm::length(vpos);

    grot = glm::dmat3(1.0); // ofsGetObjectGlobalRotation(object);

    dmWorld = glm::dmat4(grot);
    dmWorld = glm::translate(dmWorld, vpos);
}

// ******** Scene ********

vObject *Scene::addVisualObject(const Object *object)
{
    vObject *vobj = nullptr;

    vobj = vObject::create(object, *this);
    vobjList.push_back(vobj);

    return vobj;
}

vObject *Scene::getVisualObject(const Object *object, bool bCreate)
{
    for (int idx = 0; idx < vobjList.size(); idx++)
        if (vobjList[idx]->getObject() == object)
            return vobjList[idx];

    if (bCreate == true)
        return addVisualObject(object);
    return nullptr;
}