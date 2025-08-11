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

void RigidBody::setOrbitReference(Celestial *body)
{
    if (body != nullptr && body != cbody) {
        cbody = body;
        oel.setup(mass, cbody->getMass(), oel.getMJDEpoch());
        bOrbitalValid = false;
    }
}

glm::dvec3 RigidBody::computeEulerInverseZero(const glm::dvec3 &tau, const glm::dvec3 &omega)
{
    // domega/dt = 0
    return { 0, 0, 0 };
}

glm::dvec3 RigidBody::computeEulerInverseSimple(const glm::dvec3 &tau, const glm::dvec3 &omega)
{
    // domega/dt = tau
    return { tau.x/pmi.x, tau.y/pmi.y, tau.z/pmi.z };
}

glm::dvec3 RigidBody::computeEulerInverseFull(const glm::dvec3 &tau, const glm::dvec3 &omega)
{
    // domega/dt + domega x omega = tau
    return { (tau.x - (pmi.y-pmi.z) * omega.y*omega.z) / pmi.x,
             (tau.y - (pmi.z-pmi.x) * omega.z*omega.x) / pmi.y,
             (tau.z - (pmi.x-pmi.y) * omega.x*omega.y) / pmi.z };
}

void RigidBody::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double step, double dt)
{
    assert(system != nullptr);

    acc = system->addGravityIntermediate(state.pos, step, this);

    // Gravity Torque
    if (cbody != nullptr && !bIgnoreGravTorque) {
        glm::dvec3 R0 = state.Q * cbody->interpolatePosition(step) - state.pos;
        double r0 = glm::length(R0);
        glm::dvec3 Re = R0/r0;
        double mag = 3.0 * (astro::G * cbody->getMass()) / (r0*r0*r0);
        am = glm::cross(pmi*Re, Re) * mag;
    } else
        am = {};
}

void RigidBody::updateGlobal(const glm::dvec3 &rpos, const glm::dvec3 &rvel)
{
    StateVectors &s = getStateVector();
    s.pos = rposBase = rpos;
    s.vel = rvelBase = rvel;
    rposAdd = {};
    rvelAdd = {};
}

void RigidBody::update(bool force)
{
    if (bDynamicForce)
    {
        if (bOrbitNotInitialized)
        {
            flushPosition();
            flushVelocity();
            s1.pos = cpos;
            s1.vel = cvel;
            // getIntermediateMomentsPert(accp, am, s0, 0, dt, cbody);
            oel.determine(cpos, cvel, ofsDate->getSimTime0());
        }

        // Updating orbital path
        //calculateEncke();
        oel.update(ofsDate->getSimTime1(), cpos, cvel);
        // oel.calculate(cpos, cvel, ofsDate->getSimTime1());

        // ofsLogger->info("RigidBody - updates\n");
        // ofsLogger->info("{}: rpos {:.3f},{:.3f},{:.3f} ({:.3f})\n", getsName(),
        //     cpos.x, cpos.y, cpos.z, glm::length(cpos));
        // ofsLogger->info("{}: rvel {:.4f},{:.4f},{:.4f} - {:.4f} mph\n", getsName(),
        //     cvel.x, cvel.y, cvel.z, glm::length(cvel) * 3600 * 0.621);

        s1.pos = cbody->s1.pos + cpos;
        s1.vel = cbody->s1.vel + cvel;
        flushPosition();
        flushVelocity();
        s1.R = glm::dmat3(1); // glm::mat3_cast(s1.Q);
        // getIntermediateMoments(acc, am, s1, 1, dt);
        bOrbitNotInitialized = false;

        // ofsLogger->info("RigidBody - updates\n");
        // ofsLogger->info("{}: cpos {},{},{} time {}\n",
        //     getsName(), cpos.x, cpos.y, cpos.z, ofsDate->getSimTime1());
        // ofsLogger->info("{}: cvel {},{},{}\n", getsName(), cvel.x, cvel.y, cvel.z);
        // ofsLogger->info("{}: s1pos {},{},{}\n", getsName(), s1.pos.x, s1.pos.y, s1.pos.z);
        // ofsLogger->info("{}: s1vel {},{},{}\n", getsName(), s1.vel.x, s1.vel.y, s1.vel.z);
    }
    
    // Object::update(force);
    if (cbody != nullptr) {
        cpos = s1.pos - cbody->s1.pos;
        cvel = s1.vel - cbody->s1.vel;
    }
}