// vessel.cpp - Vessel (spacecraft) package
//
// Author:  Tim Stark
// Date:    Apr 24, 2022

#include "main/core.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "engine/vessel.h"
#include "universe/astro.h"

void surface_t::setLanded(double _lng, double _lat, double alt, double dir,
    const vec3d_t &nml, const Object *object)
{
    static const double eps = 1e-6;

    body = object;

    lng = _lng;
    lat = _lat;
    altitude = alt;
    heading = dir;

    pitch = atan2(nml.z(), nml.y());
    bank = (fabs(nml.x()) > eps || fabs(nml.y()) > eps)
        ? atan2(nml.x(), nml.y()) : 0.0;

    airSpeed = groundSpeed = 0.0;

    slng = sin(lng), clng = cos(lng);
    slat = sin(lat), clat = cos(lat);
    // l2h = { -slng,      0.0,   clng,
    //          clat*clng, slat,  clat*slng,
    //         -slat*clng, clat, -slat*slng };

}

VesselBase::VesselBase()
: RigidBody("", objVessel)
{
    
}

Vessel::Vessel()
{

}

void Vessel::setGenericDefaults()
{
    // Object stands above 2 meters with 3 legs
    tdVertex_t tdvtx[3];
    tdvtx[0].pos = {  0, -2,  2 }; // forward leg
    tdvtx[1].pos = { -2, -2, -2 }; // left back leg
    tdvtx[2].pos = {  2, -2, -2 }; // right back leg
    for (int idx = 0; idx < sizeof(tdvtx); idx++)
    {
        tdvtx[idx].stiffness = 0;
        tdvtx[idx].damping = 0;
    }

    setTouchdownPoints(tdvtx, sizeof(tdvtx));

}

void Vessel::setTouchdownPoints(const tdVertex_t *tdvtx, int ntd)
{
    // Number of touchdown points must be least 3 points
    assert(ntd >= 3);

    // Initialize touchdown points
    tpVertices.clear();
    tpVertices.resize(ntd);

    for (int idx = 0; idx < ntd; idx++)
    {
        tpVertices[idx].pos = tdvtx[idx].pos / M_PER_KM;
        tpVertices[idx].stiffness = tdvtx[idx].stiffness;
        tpVertices[idx].damping = tdvtx[idx].damping;
    }

    vec3d_t tp[3] = { tpVertices[0].pos, tpVertices[1].pos, tpVertices[2].pos };

    tpNormal = (tp[0] - (tp[1] + tp[2])*0.5).cross(tp[2] - tp[1]);
    double len = tpNormal.norm();
    tpNormal /= len;

    double a = tp[0].y() * (tp[1].z() - tp[2].z()) - tp[1].y() * (tp[0].z() - tp[2].z()) + tp[2].y() * (tp[0].z() - tp[1].z());
    double b = tp[0].x() * (tp[1].z() - tp[2].z()) - tp[1].x() * (tp[0].z() - tp[2].z()) + tp[2].x() * (tp[0].z() - tp[1].z());
    double c = tp[0].x() * (tp[1].y() - tp[2].y()) - tp[1].x() * (tp[0].y() - tp[2].y()) + tp[2].x() * (tp[0].y() - tp[1].y());
    double d = -tp[0].x()*a - tp[0].y()*b - tp[0].z()*c;
    double scl = sqrt(a*a + b*b + c*c);

    cogElev = fabs(d / scl);
    tpCGravity = { -a*cogElev/scl, -b*cogElev/scl, -c*cogElev/scl };

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

        // landrot = {  sp->clng*sp->slat*sdir - sp->lng*cdir,   sp->clng*sp->clat,  -sp->clng*sp->slat*cdir - sp->slng*sdir,
        //             -sp->clat*sdir,                           sp->slat,            sp->clat*cdir,
        //              sp->slng*sp->slat*sdir + sp->clng*cdir,  sp->clat*sp->slng,  -sp->slng*sp->slat*cdir + sp->clng*sdir }; 
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