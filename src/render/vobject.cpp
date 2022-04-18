// vobject.cpp - Visual Object Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#include "main/core.h"
#include "osd/gl/mesh.h"
#include "render/scene.h"
#include "render/vobject.h"
#include "render/vbody.h"
#include "render/vstar.h"
#include "render/vvessel.h"

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