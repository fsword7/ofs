// ofsapi.cpp - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/camera.h"
#include "engine/object.h"
#include "main/app.h"
#include "universe/universe.h"

LIBEXPORT glm::dvec3 ofsGetCameraGlobalPosition()
{
    return ofsAppCore->getCamera()->getGlobalPosition();
}

LIBEXPORT glm::dvec3 ofsGetCameraGlobalDirection()
{
    return ofsAppCore->getCamera()->getGlobalDirection();
}

LIBEXPORT glm::dmat3 ofsGetCameraRotationMatrix()
{
    return ofsAppCore->getCamera()->getGlobalRotation();
}

LIBEXPORT ObjectHandle ofsGetObjectByName(cstr_t &name)
{
    return ofsAppCore->getUniverse()->findPath(name);
}

LIBEXPORT ObjectType ofsGetObjectType(ObjectHandle object)
{
    return static_cast<Object *>(object)->getType();
}

LIBEXPORT double ofsGetObjectRadius(ObjectHandle object)
{
    return static_cast<Object *>(object)->getRadius(); 
}

LIBEXPORT glm::dvec3 ofsGetObjectGlobalPosition(ObjectHandle object)
{
    return glm::dvec3(0, 0, 0);
}