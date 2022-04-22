// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "engine/object.h"

vec3d_t Object::getuPosition(double tjd)
{
    return objPosition;
}

quatd_t Object::getuOrientation(double tjd)
{
    return objRotation;
}
