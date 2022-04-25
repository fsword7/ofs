// vessel.cpp - Vessel (spacecraft) package
//
// Author:  Tim Stark
// Date:    Apr 24, 2022

#include "main/core.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "engine/vessel.h"

void surface_t::setLanded(double _lng, double _lat, double alt, double dir,
    const vec3d_t &nml, const Object *object)
{
    static const double eps = 1e-6;

    body = object;

    lng = _lng;
    lat = _lat;
    altitude = alt;
    heading = dir;

    pitch = atan2(nml.z, nml.y);
    bank = (fabs(nml.x) > eps || fabs(nml.y) > eps)
        ? atan2(nml.x, nml.y) : 0.0;

    airSpeed = groundSpeed = 0.0;

    slng = sin(lng), clng = cos(lng);
    slat = sin(lat), clat = cos(lat);
    l2h = { -slng,      0.0,   clng,
             clat*clng, slat,  clat*slng,
            -slat*clng, clat, -slat*slng };

}

VesselBase::VesselBase()
: RigidBody("", objVessel)
{
    
}

Vessel::Vessel()
{

}

void Vessel::initLanded(Object *object, double lat, double lng, double dir)
{

    // double slng = sin(lng), clng = cos(lng);
    // double slat = sin(lat), clat = cos(lat);
    // mat3d_t loc = { -slng,      0,     clng,
    //                  clat*clng, slat,  clat*slng,
    //                 -slat*clng, clat, -slat*slng };

    surface_t *sp = &surfParam;

    {
        double sdir = sin(dir), cdir = cos(dir);

        landrot = {  sp->clng*sp->slat*sdir - sp->lng*cdir,   sp->clng*sp->clat,  -sp->clng*sp->slat*cdir - sp->slng*sdir,
                    -sp->clat*sdir,                           sp->slat,            sp->clat*cdir,
                     sp->slng*sp->slat*sdir + sp->clng*cdir,  sp->clat*sp->slng,  -sp->slng*sp->slat*cdir + sp->clng*sdir }; 
    }


    fsType = fsLanded;
}

void Vessel::initDocked()
{
    
}

void Vessel::initOrbiting()
{

}

bool Vessel::addSurfaceForces(vec3d_t &acc, vec3d_t &am, const StateVectors &state, double dt)
{
    return false;
}

void Vessel::getIntermediateMoments(vec3d_t &acc, vec3d_t &am, const StateVectors &state, double dt)
{
    vec3d_t F = Fadd;
    vec3d_t M = Ladd;

    // Check for collision detection
    addSurfaceForces(F, M, state, dt);

    // Updates linear and angular moments
    acc += state.Q * F/mass;
    am  += M/mass;
}

void Vessel::updateMass()
{
    pfmass = fmass;
    fmass = 0.0;
    // for (int idx; idx < nTanks; idx++)
    //     fmass += tanks[idx]->mass;
    mass = emass + fmass;
}

void Vessel::update()
{

    if (fsType == fsFlight)
    {
        RigidBody::update();
    } 
    else if (fsType == fsLanded)
    {

    }

    // Update position and velocity in orbit reference frame
    cpos = s1.pos - cbody->s1.pos;
    cvel = s1.vel - cbody->s1.vel;

    // Reset linear and angular forces
    // for next update phase
    F = Fadd;
    L = Ladd;
    F = { 0, 0, 0 };
    L = { 0, 0, 0 };

    // Update mass for fuel consumption
    updateMass();
}