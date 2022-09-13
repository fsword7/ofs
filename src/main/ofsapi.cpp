// ofsapi.cpp - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/object.h"
#include "main/app.h"
#include "universe/universe.h"

LIBEXPORT ObjectHandle ofsGetObjectByName(cstr_t &name)
{
    return ofsAppCore->getUniverse()->findPath(name);
}

LIBEXPORT ObjectType ofsGetObjectType(ObjectHandle object)
{
    return static_cast<Object *>(object)->getType();
}