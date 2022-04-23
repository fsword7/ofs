// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "engine/object.h"

vec3d_t Object::getuPosition(double tjd) const
{
    return objPosition;
}

vec3d_t Object::getoPosition(double tjd) const
{
    return objPosition;
}

quatd_t Object::getuOrientation(double tjd) const
{
    return objRotation;
}
