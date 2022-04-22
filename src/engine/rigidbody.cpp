// rigidbody.cpp - Rigid Body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "ephem/orbit.h"
#include "universe/frame.h"

vec3d_t RigidBody::getuPosition(double tjd)
{
    vec3d_t pos = { 0, 0, 0 };
    vec3d_t opos = orbit->getPosition(tjd);
    Frame  *frame = orbitFrame;

    while (frame->getCenter()->getType() == objCelestialBody)
    {
        Object *object = frame->getCenter();

        pos  += frame->getOrientation(tjd) * opos;
        orbit = object->getOrbit();
        frame = object->getOrbitFrame();
        opos  = orbit->getPosition(tjd);
    }

    pos += frame->getOrientation(tjd) * opos;

    return pos;

    // return { 0, 0, 0 };
}

quatd_t RigidBody::getuOrientation(double tjd)
{
    return { 1, 0, 0, 0 };
}

vec3d_t RigidBody::getoPosition(double tjd)
{
    return orbit->getPosition(tjd);
}