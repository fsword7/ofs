// rigidbody.cpp - Rigid Body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "ephem/orbit.h"
#include "ephem/rotation.h"
#include "universe/psystem.h"
#include "universe/frame.h"

RigidBody::RigidBody(cjson &config, ObjectType type, celType celtype)
: Celestial(config, type, celtype)
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

void RigidBody::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double step, double dt)
{
    // pSystem *system = nullptr;

    assert(system != nullptr);

    acc = system->addGravityIntermediate(state.pos, step, this);
    am = {};

    if (cbody != nullptr)
    {
        glm::dvec3 R0 = state.Q * cbody->interpolatePosition(step) - state.pos;
        double r0 = glm::length(R0);
        glm::dvec3 Re = R0/r0;
        double mag = 3.0 * (astro::G * cbody->getMass()) / (r0*r0*r0);
        am = glm::cross(pmi*Re, Re) * mag;
    }
}

void RigidBody::updateGlobal(const glm::dvec3 &rpos, const glm::dvec3 &rvel)
{
    s0.pos = rposBase = rpos;
    s0.vel = rvelBase = rvel;
    rposAdd = {};
    rvelAdd = {};
}

void RigidBody::update(bool force)
{
    // {
    //     {
    //         flushPosition();
    //         flushVelocity();
    //         s1.pos = cpos;
    //         s1.vel = cvel;
    //         // getIntermediateMomentsPert(accp, am, s0, 0, dt, cbody);
    //         oel.calculate(cpos, cvel, ofsDate->getSimTime0());
    //     }
    //     //calculateEncke();
    //     s1.pos = cbody->s1.pos + cpos;
    //     s1.vel = cbody->s1.vel + cvel;
    //     flushPosition();
    //     flushVelocity();
    //     s1.R = glm::mat3_cast(s1.Q);
    //     getIntermediateMoments(acc, am, s1, 1, dt);
    // }
    
    Object::update(force);
}