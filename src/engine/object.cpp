// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "engine/object.h"

glm::dvec3 Object::getuPosition(double tjd) const
{
    return objPosition;
}

glm::dvec3 Object::getoPosition(double tjd) const
{
    return objPosition;
}

glm::dmat3 Object::getuOrientation(double tjd) const
{
    return objRotation;
}

void Object::updateCullingRadius()
{
    cullingRadius = getBoundingRadius();
}
