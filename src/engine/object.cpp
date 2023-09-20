// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#define OPSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/object.h"

void Object::update(bool force)
{
}

void Object::beginUpdate()
{
}

void Object::endUpdate()
{
    s0 = s1; // Move all to S0
    s1 = {}; // Clear all for next update
}

glm::dvec3 Object::getuPosition(double tjd) const
{
    return objPosition;
}

glm::dvec3 Object::getuVelocity(double tjd) const
{
    return objPosition;
}

glm::dmat3 Object::getuOrientation(double tjd) const
{
    return objRotation;
}


glm::dvec3 Object::getoPosition(double tjd) const
{
    return objPosition;
}

glm::dvec3 Object::getoVelocity(double tjd) const
{
    return objVelocity;
}

void Object::updateCullingRadius()
{
    cullingRadius = getBoundingRadius();
}
