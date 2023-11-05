// rigidbody.cpp - Rigid Body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "ephem/orbit.h"
#include "ephem/rotation.h"
#include "universe/frame.h"

RigidBody::RigidBody(json &cfg, ObjectType type)
: Object(cfg, type)
{

}

// glm::dvec3 RigidBody::getuPosition(double tjd) const
// {
//     if (getType() == objCelestialStar)
//         return { 0, 0, 0 };

//     glm::dvec3 pos = { 0, 0, 0 };
//     glm::dvec3 opos = getoPosition(tjd);
//     Frame  *frame = orbitFrame;

//     // fmt::printf("%s: Position: (%lf,%lf,%lf)\n", getsName(), opos.x, opos.y, opos.z);

//     while (frame->getCenter()->getType() == objCelestialBody)
//     {
//         Object *object = frame->getCenter();

//         pos  += frame->getOrientation(tjd) * opos;
//         opos  = object->getoPosition(tjd);
//         frame = object->getOrbitFrame();

//         // fmt::printf("%s: Position: (%lf,%lf,%lf)\n", object->getsName(), opos.x, opos.y, opos.z);
//     }

//     pos += frame->getOrientation(tjd) * opos;

//     // fmt::printf("Final Position: (%lf,%lf,%lf)\n", pos.x, pos.y, pos.z);

//     return pos;
// }

// glm::dmat3 RigidBody::getuOrientation(double tjd) const
// {
//     return glm::dmat3(1.0); // rotation != nullptr ? rotation->getRotation(tjd) : quatd_t( 1, 0, 0, 0 );
// }

// glm::dvec3 RigidBody::getoPosition(double tjd) const
// {
//     return orbit->getPosition(tjd);
// }

void RigidBody::update(bool force)
{

    Object::update(force);
}