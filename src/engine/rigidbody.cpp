// rigidbody.cpp - Rigid Body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "ephem/orbit.h"
#include "ephem/rotation.h"
#include "universe/frame.h"

vec3d_t RigidBody::getuPosition(double tjd) const
{
    if (getType() == objCelestialStar)
        return { 0, 0, 0 };

    vec3d_t pos = { 0, 0, 0 };
    vec3d_t opos = getoPosition(tjd);
    Frame  *frame = orbitFrame;

    // fmt::printf("%s: Position: (%lf,%lf,%lf)\n", getsName(), opos.x, opos.y, opos.z);

    while (frame->getCenter()->getType() == objCelestialBody)
    {
        Object *object = frame->getCenter();

        pos  += frame->getOrientation(tjd) * opos;
        opos  = object->getoPosition(tjd);
        frame = object->getOrbitFrame();

        // fmt::printf("%s: Position: (%lf,%lf,%lf)\n", object->getsName(), opos.x, opos.y, opos.z);
    }

    pos += frame->getOrientation(tjd) * opos;

    // fmt::printf("Final Position: (%lf,%lf,%lf)\n", pos.x, pos.y, pos.z);

    return pos;
}

quatd_t RigidBody::getuOrientation(double tjd) const
{
    return rotation != nullptr ? rotation->getRotation(tjd) : quatd_t( 1, 0, 0, 0 );
}

vec3d_t RigidBody::getoPosition(double tjd) const
{
    return orbit->getPosition(tjd);
}

void RigidBody::update()
{
}