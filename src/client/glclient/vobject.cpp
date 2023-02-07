// vobject.cpp - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#include "main/core.h"
#include "api/ofsapi.h"
#include "client.h"
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

void vObject::update(int now)
{

    gpos  = ofsGetObjectGlobalPosition(object, now);
    vpos  = gpos - scene.getCamera()->getGlobalPosition();
    vdist = glm::length(vpos);

    grot = glm::dmat3(1.0); // ofsGetObjectGlobalRotation(object);
    dmWorld = {
        { grot[0][0], grot[0][1], grot[0][2], 0 },
        { grot[1][0], grot[1][1], grot[1][2], 0 },
        { grot[2][0], grot[2][1], grot[2][2], 0 },
        { vpos.x, vpos.y, vpos.z, 1.0}
    };

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