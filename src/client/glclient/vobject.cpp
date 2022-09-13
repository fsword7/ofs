// vobject.cpp - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#include "main/core.h"
#include "api/ofsapi.h"
#include "scene.h"
#include "vobject.h"
#include "vbody.h"
#include "vstar.h"
#include "vvessel.h"


vObject *vObject::create(ObjectHandle object, Scene &scene)
{
    switch (ofsGetObjectType(object))
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

// ******** Scene ********

vObject *Scene::addVisualObject(ObjectHandle object)
{
    vObject *vobj = nullptr;

    vobj = vObject::create(object, *this);
    vobjList.push_back(vobj);

    return vobj;
}

vObject *Scene::getVisualObject(ObjectHandle object, bool bCreate)
{
    for (int idx = 0; idx < vobjList.size(); idx++)
        if (vobjList[idx]->getObject() == &object)
            return vobjList[idx];

    if (bCreate == true)
        return addVisualObject(object);
    return nullptr;
}