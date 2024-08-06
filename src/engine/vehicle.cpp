// vehicle.cpp - Vehicle package
//
// Author:  Tim Stark
// Date:    Apr 24, 2022

#include "main/core.h"
#include "api/draw.h"
#include "control/hudpanel.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "engine/vehicle.h"
#include "universe/astro.h"

void surface_t::setLanded(double _lng, double _lat, double alt, double dir,
    const glm::dvec3 &nml, const Object *object)
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
    // l2h = { -slng,      0.0,   clng,
    //          clat*clng, slat,  clat*slng,
    //         -slat*clng, clat, -slat*slng };

}

// ******** VehicleBase ********

VehicleBase::VehicleBase(cstr_t &name)
: RigidBody(name, objVehicle)
{
}

VehicleBase::VehicleBase(YAML::Node &config)
: RigidBody(config, objVehicle)
{
}

// void VehicleBase::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double dt)
// {
// }

bool VehicleBase::addSurfaceForces(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt)
{
    return false;
}

// ******** Vehicle ********

Vehicle::Vehicle(cstr_t &name)
: VehicleBase(name)
{
    setGenericDefaults();
    thgrpList.reserve(thgMaxThrusters);
    for (int idx = 0; idx < thgMaxThrusters; idx++)
        thgrpList[idx] = nullptr;
}

Vehicle::Vehicle(YAML::Node &config)
: VehicleBase(config)
{
    thgrpList.reserve(thgMaxThrusters);
    for (int idx = 0; idx < thgMaxThrusters; idx++)
        thgrpList[idx] = nullptr;
}

Vehicle::~Vehicle()
{
    if (vif.module != nullptr)
        clearModule();
}

void Vehicle::clearModule()
{
    if (vif.module != nullptr && vif.ovcExit != nullptr)
        vif.ovcExit(vif.module);
    if (handle != nullptr)
        ofsUnloadModule(handle);
    handle = nullptr;

    // Clear all rehgister module
    vif.module  = nullptr;
    vif.version = 0;
    vif.ovcInit = nullptr;
    vif.ovcExit = nullptr;
}

bool Vehicle::registerModule(cstr_t &name)
{
    assert(handle == nullptr);
    cstr_t path = "modules/Vehicles/";

#ifdef __WIN32__
    std::string fname = fmt::format("{}/lib{}.dll", path, name);
#else /* __WIN32__ */
    std::string fname = fmt::format("{}/lib{}.so", path, name);
#endif /* __WIN32__ */
    handle = ofsLoadModule(fname.c_str());
    if (handle == nullptr)
    {
        printf("Failed loading module %s: %s\n",
            name.c_str(), ofsGetModuleError());      
        return false;
    }

    int (*fncVersion)() = (int(*)())ofsGetProcAddress(handle, "getModuleVersion");

    vif.version = (fncVersion ? fncVersion() : 0);
    vif.ovcInit = (ovcInit_t)ofsGetProcAddress(handle, "ovcInit");
    vif.ovcExit = (ovcExit_t)ofsGetProcAddress(handle, "ovcExit");

    return true;
}

bool Vehicle::loadModule(cstr_t &name)
{
    clearModule();

    if (!registerModule(name))
    {
        char *code = dlerror();
        ofsLogger->error("Can't load vehicle module: {} (code: {})\n", name, code);
        return false;
    }

    if (vif.ovcInit != nullptr)
        vif.module = vif.ovcInit(this);
    
    return true;
}

void Vehicle::setGenericDefaults()
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

void Vehicle::setTouchdownPoints(const tdVertex_t *tdvtx, int ntd)
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

    glm::dvec3 tp[3] = { tpVertices[0].pos, tpVertices[1].pos, tpVertices[2].pos };

    tpNormal = glm::cross(tp[0] - (tp[1] + tp[2])*0.5, tp[2] - tp[1]);
    double len = glm::length(tpNormal);
    tpNormal /= len;

    double a = tp[0].y * (tp[1].z - tp[2].z) - tp[1].y * (tp[0].z - tp[2].z) + tp[2].y * (tp[0].z - tp[1].z);
    double b = tp[0].x * (tp[1].z - tp[2].z) - tp[1].x * (tp[0].z - tp[2].z) + tp[2].x * (tp[0].z - tp[1].z);
    double c = tp[0].x * (tp[1].y - tp[2].y) - tp[1].x * (tp[0].y - tp[2].y) + tp[2].x * (tp[0].y - tp[1].y);
    double d = -tp[0].x*a - tp[0].y*b - tp[0].z*c;
    double scl = sqrt(a*a + b*b + c*c);

    cogElev = fabs(d / scl);
    tpCGravity = { -a*cogElev/scl, -b*cogElev/scl, -c*cogElev/scl };

}

void Vehicle::initLanded(Object *object, double lat, double lng, double dir)
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

        // Set rotation matrix for local horizon frame
        // for right-handed rule (OpenGL). Points
        // to east as origin at (0, 0).
        //
        //     |  slat  clat   0  | |  clng   0   slng |
        // R = | -clat  slat   0  | |   0     1    0   |
        //     |   0     0     1  | | -slng   0   clng |
        //
        // double clat = cos(lat), slat = sin(lat);
        // double clng = cos(lng), slng = sin(lng);
        // go.R = { slat*clng,  clat*clng, slng,
        //         -clat,       slat,      0,
        //         -slat*slng, -clat*slng, clng };
        // go.Q = go.R;

    }


    fsType = fsLanded;
}

void Vehicle::initOrbiting(const glm::dvec3 &pos, const glm::dvec3 &vel, const glm::dvec3 &rot, const glm::dvec3 *vrot)
{
    // Assign current position/velocity
    cpos = pos, cvel = vel;

    oel.calculate(cpos, cvel, ofsDate->getSimTime0());

    // Make sure that we are above ground (sanity check)
    double rad = glm::length(cpos);
    double elev = 0.0;
    if (rad < cbody->getRadius() + elev)
    {
        double scale = (cbody->getRadius() + elev) / rad;
        cpos *= scale;
    }

    // s0.R = rot;
    s0.Q = s0.R;
    if (vrot != nullptr)
        s0.omega = *vrot;
}

void Vehicle::initDocked()
{
    
}

void Vehicle::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt)
{
    glm::dvec3 F = Fadd;
    glm::dvec3 M = Ladd;

    // Check for surface forces and collision detection
    addSurfaceForces(F, M, state, tfrac, dt);

    // Computing with N-body gravitional pull.
    RigidBody::getIntermediateMoments(acc, am, state, tfrac, dt);

    // Update linear and angular moments in vehicle reference frame
    acc += state.Q * F/mass;
    am  += M/mass;
}

bool Vehicle::addSurfaceForces(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt)
{
    return false;
}

void Vehicle::updateMass()
{
    pfmass = fmass;
    fmass = 0.0;
    // for (int idx; idx < nTanks; idx++)
    //     fmass += tanks[idx]->mass;
    mass = emass + fmass;
}

void Vehicle::update(bool force)
{

    if (fsType == fsFlight)
    {
        RigidBody::update(force);
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

void Vehicle::updateBodyForces()
{
    // updateAerodynamicForces();
    updateThrustForces();
}

void Vehicle::updateAerodynamicForces()
{

}

void Vehicle::updateThrustForces()
{

    glm::dvec3 thrust = { 0, 0, 0 };
    glm::dvec3 F;

    for (auto th : thrustList)
    {
        th->level = std::max(0.0, std::min(1.0, th->lvperm + th->lvover));
        if (th->level > 0.0)
        {   
            tank_t *tank = th->tank;

            double th0 = th->level * th->maxth;
            F = th->dir * th0;
            thrust += F;
            camom += glm::cross(F, th->pos);
        }
        th->lvover = 0.0;
    }

    cflin += thrust;
}

void Vehicle::drawHUD(HUDPanel *hud, Sketchpad *pad)
{
    hud->drawDefault(pad);
}

// Thruster 

void Vehicle::createThruster(const glm::dvec3 &pos, const glm::dvec3 &dir, double maxth, tank_t *tank)
{

}

bool Vehicle::deleteThruster(thrust_t *th)
{
    
    for (auto it = thrustList.begin(); it != thrustList.end(); it++)
    {
        if (*it == th)
        {
            it = thrustList.erase(it);
            return true;
        }
    }

    return false;
}

// void Vehicle::setThrustLevel(thrust_t *th, double level)
// {
//     double dlevel = level - th->lvperm;
//     th->lvperm = level;
//     if (th->tank != nullptr && th->tank->mass > 0)
//         th->level = std::max(0.0, std::min(1.0, th->level + dlevel));
// }

// void Vehicle::adjustThrustLevel(thrust_t *th, double dlevel)
// {
//     th->lvperm += dlevel;
//     if (th->tank != nullptr && th->tank->mass > 0)
//         th->level = std::max(0.0, std::min(1.0, th->level + dlevel));
// }

// void Vehicle::setThrustOverride(thrust_t *th, double level)
// {
//     th->lvover = level;
// }

// void Vehicle::adjustThrustOverride(thrust_t *th, double dlevel)
// {
//     th->lvover += dlevel;
// }

// Thruster Group

void Vehicle::createThrusterGroup(thrustgrp_t *tg, thrustType_t type)
{
    
}


void Vehicle::setThrustGroupLevel(thrustgrp_t *tg, double level)
{
    for (auto th : tg->thrusters)
        th->setThrustLevel(level);
}

void Vehicle::adjustThrustGroupLevel(thrustgrp_t *tg, double dlevel)
{
    for (auto th : tg->thrusters)
        th->adjustThrustLevel(dlevel);
}

void Vehicle::setThrustGroupOverride(thrustgrp_t *tg, double level)
{
    for (auto th : tg->thrusters)
        th->setThrustOverride(level);
}

void Vehicle::adjustThrustGroupOverride(thrustgrp_t *tg, double dlevel)
{
    for (auto th : tg->thrusters)
        th->adjustThrustOverride(dlevel);
}


void Vehicle::setThrustGroupLevel(thrustType_t type, double level)
{
    setThrustGroupLevel(thgrpList[type], level);
}

void Vehicle::adjustThrustGroupLevel(thrustType_t type, double dlevel)
{
    adjustThrustGroupLevel(thgrpList[type], dlevel);
}

void Vehicle::setThrustGroupOverride(thrustType_t type, double level)
{
    setThrustGroupOverride(thgrpList[type], level);
}

void Vehicle::adjustThrustGroupOverride(thrustType_t type, double dlevel)
{
    setThrustGroupOverride(thgrpList[type], dlevel);
}


double Vehicle::getThrustGroupLevel(thrustgrp_t *tg)
{
    double level = 0.0;
    for (auto th : tg->thrusters)
        level += th->level;
    return tg->thrusters.size() > 0 ? level / tg->thrusters.size() : 0.0;
}

