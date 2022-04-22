// rigidbody.cpp - Rigid Body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "ephem/orbit.h"

vec3d_t RigidBody::getuPosition(double tjd)
{
    return { 0, 0, 0 };
}

quatd_t RigidBody::getuOrientation(double tjd)
{
    return { 1, 0, 0, 0 };
}

vec3d_t RigidBody::getoPosition(double tjd)
{
    return orbit->getPosition(tjd);
}