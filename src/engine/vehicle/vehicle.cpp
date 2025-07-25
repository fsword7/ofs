// vehicle.cpp - Vehicle package
//
// Author:  Tim Stark
// Date:    Apr 24, 2022

#include "main/core.h"
#include "api/draw.h"
#include "control/hud/panel.h"
#include "ephem/elements.h"
#include "engine/celestial.h"
#include "engine/rigidbody.h"
#include "engine/mesh.h"
#include "engine/vehicle/vehicle.h"
#include "universe/astro.h"
#include "universe/body.h"

void surface_t::setLanded(double _lng, double _lat, double _alt, double dir,
    const glm::dvec3 &nml, const Celestial *object)
{
    static const double eps = 1e-6;
    const CelestialPlanet *planet = dynamic_cast<const CelestialPlanet *>(object);

    body = object;

    lng = _lng;
    lat = _lat;
    alt = _alt;
    heading = dir;
    snml = { 0, 1, 0 };

    // Set all speed to zero
    gvglob = { 0, 0, 0 };
    gvlocal = { 0, 0, 0 };
    avglob = { 0, 0, 0 };
    avlocal = { 0, 0, 0 };
    airSpeed = 0.0;
    groundSpeed = 0.0;

    pitch = atan2(nml.z, nml.y);
    bank = (fabs(nml.x) > eps || fabs(nml.y) > eps)
        ? atan2(nml.x, nml.y) : 0.0;

    airSpeed = groundSpeed = 0.0;

    slng = sin(lng), clng = cos(lng);
    slat = sin(lat), clat = cos(lat);


    //     | -slng   0.0   clng | |  1.0   0.0   0.0  |
    // R = |  0.0    1.0   0.0  | |  0.0  slat  clat  |
    //     |  clng   0.0  -slng | |  0.0  clat -slat  |

    // l2h = { -slng,      0.0,   clng,
    //          clat*clng, slat,  clat*slng,
    //         -slat*clng, clat, -slat*slng };

    // l2h = { -slng, clat*clng, -slat*clng,
    //         0.0,   slat,       clat,
    //         clng,  clat*slng, -slat*slng };

    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0).
    //
    //     |  slat  clat   0  | |  clng   0   slng |
    // R = | -clat  slat   0  | |   0     1    0   |
    //     |   0     0     1  | | -slng   0   clng |

    double clat = cos(lat), slat = sin(lat);
    double clng = cos(lng), slng = sin(lng);
    R =    { slat*clng,  clat*clng, slng,
            -clat,       slat,      0,
            -slat*slng, -clat*slng, clng };
    Q = R;

    isInAtomsphere = (planet != nullptr) && (planet->hasAtmosphere());
    if (isInAtomsphere)
    {
        atmprm_t prm;
        planet->getAtmParam({lat, lng, alt0}, &prm);
        atmPressure = prm.p;
        atmDensity = prm.rho;
        atmTemp = prm.T;
        atmMach = planet->getSoundSpeed(atmTemp);
    }
}

void surface_t::setLanded(const glm::dvec3 &loc, double dir, const Celestial *object)
{
    static const double eps = 1e-6;

    body = object;

    lat = loc.x;
    lng = loc.y;
    alt = loc.z;
    heading = dir;
    snml = { 0, 1, 0};

    // Set all speed to zero
    gvglob = { 0, 0, 0 };
    gvlocal = { 0, 0, 0 };
    avglob = { 0, 0, 0 };
    avlocal = { 0, 0, 0 };
    airSpeed = 0.0;
    groundSpeed = 0.0;

    // pitch = atan2(nml.z, nml.y);
    // bank = (fabs(nml.x) > eps || fabs(nml.y) > eps)
    //     ? atan2(nml.x, nml.y) : 0.0;

    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0).
    //
    //     |  slat  clat   0  | |  clng   0   slng |
    // R = | -clat  slat   0  | |   0     1    0   |
    //     |   0     0     1  | | -slng   0   clng |

    clat = cos(lat), slat = sin(lat);
    clng = cos(lng), slng = sin(lng);
    R =    { slat*clng,  clat*clng, slng,
            -clat,       slat,      0,
            -slat*slng, -clat*slng, clng };
    Q = R;
}

void surface_t::update(const StateVectors &s, const StateVectors &os, const Celestial *object,
    elevTileList_t *etile)
{
    const CelestialPlanet *planet = dynamic_cast<const CelestialPlanet *>(object);
    static const double eps = 1e-4;

    // Get latitude and longtitude from local planet coordinate
    glm::vec3 rpos = s.pos - os.pos;
    ploc = glm::transpose(os.R) * rpos;
    // object->convertGlobalToLocal(vState.pos, ploc);
    object->convertLocalToEquatorial(ploc, lat, lng, rad);

    alt0 = rad - planet->getRadius();
    elev = 0.0, alt = alt0;
    snml = { 0, 1, 0};
    
    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0).
    //
    //     |  slat  clat   0  | |  clng   0   slng |
    // R = | -clat  slat   0  | |   0     1    0   |
    //     |   0     0     1  | | -slng   0   clng |

    slat = sin(lat), clat = cos(lat);
    slng = sin(lng), clng = cos(lng);
    R =    { slat*clng,  clat*clng, slng,
            -clat,       slat,      0,
            -slat*slng, -clat*slng, clng };
    Q = R;

    // Determine ground elevation
    if (etile != nullptr && alt0 < 1e5 && planet != nullptr && planet->getType() == objCelestialBody)
    {
        ElevationManager *emgr = planet->getElevationManager();
        if (emgr != nullptr)
        {
            int rlod = int(32.0 - log(std::max(alt0, 0.1))*(1.0 / log(2.0)));
            elev = emgr->getElevationData(ploc, rlod, etile);
            alt -= elev;
        }
    }

    // Update vehicle orientation at horizon level
    glm::vec3 nml = glm::normalize(glm::transpose(s.R) * rpos);
    pitch = asin(nml.z);
    bank = (fabs(nml.x) > eps) && (fabs(nml.y) > eps)
        ? atan2(-nml.x, nml.y) : 0.0;

    // Ground speed
    glm::dvec3 vrel = s.vel - os.vel;
    double period = planet->getRotationPeriod();
    double vobj = period != 0.0 ? ((pi2 / period) * rad * clat) : 0.0;
    gvglob = vrel - (s.R * glm::dvec3(-vobj*slng, 0.0, vobj*clng));
    gvlocal = glm::transpose(s.R) * gvglob;
    groundSpeed = glm::length(gvglob);

    // Wind speed
    wvglob = {0, 0, 0};

    // Air speed
    avglob = gvglob - wvglob;
    avlocal = glm::transpose(s.R) * avglob;
    airSpeed = glm::length(avglob);

    // Updating current atomsphere parameters
    isInAtomsphere = (planet != nullptr) && (planet->hasAtmosphere());
    if (isInAtomsphere)
    {
        atmprm_t prm;
        planet->getAtmParam({lat, lng, alt0}, &prm);
        atmPressure = prm.p;
        atmDensity = prm.rho;
        atmTemp = prm.T;
        atmMach = planet->getSoundSpeed(atmTemp);
    }
}

// ******** VehicleBase ********

VehicleBase::VehicleBase(cstr_t &name)
: RigidBody(name, objVehicle, cbVehicle)
{
    etile.reserve(2);
}

VehicleBase::VehicleBase(cjson &config)
: RigidBody(config, objVehicle, cbVehicle)
{
    etile.reserve(2);
}

// void VehicleBase::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double dt)
// {
// }

void VehicleBase::updateSurfaceParam()
{
    surfParam.update(s1, cbody->s1, cbody, &etile);
}

bool VehicleBase::addSurfaceForces(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt)
{
    return false;
}

void VehicleBase::updatePost()
{
}

// ******** Vehicle ********

Vehicle::Vehicle(cstr_t &name)
: VehicleBase(name)
{
    setGenericDefaults();
    thgrpList.resize(thgMaxThrusters);
    for (int idx = 0; idx < thgMaxThrusters; idx++)
        thgrpList[idx] = nullptr;
}

Vehicle::Vehicle(cjson &config, Celestial *object)
: VehicleBase(config)
{
    setGenericDefaults();
    thgrpList.resize(thgMaxThrusters);
    for (int idx = 0; idx < thgMaxThrusters; idx++)
        thgrpList[idx] = nullptr;

    configure(config, object);
}

Vehicle::~Vehicle()
{
    if (vif.module != nullptr)
        clearModule();
}

bool Vehicle::registerModule(cstr_t &name)
{
    assert(handle == nullptr);
    cstr_t path = OFS_LIB_VEHICLE_DIR;

#ifdef __WIN32__
    std::string fname = fmt::format("{}/lib{}.dll", path, name);
#else /* __WIN32__ */
    std::string fname = fmt::format("{}/lib{}.so", path, name);
#endif /* __WIN32__ */
    handle = ofsLoadModule(fname.c_str());
    if (handle == nullptr)
    {
        ofsLogger->error("{}: Failed loading module {}: {}\n",
            getsName(), name, ofsGetModuleError());      
        return false;
    }

    int (*fncVersion)() = (int(*)())ofsGetProcAddress(handle, "getModuleVersion");

    vif.version = (fncVersion ? fncVersion() : 0);
    vif.ovcInit = (ovcInit_t)ofsGetProcAddress(handle, "ovcInit");
    vif.ovcExit = (ovcExit_t)ofsGetProcAddress(handle, "ovcExit");

    return true;
}

void Vehicle::clearModule()
{
    if (vif.module != nullptr && vif.ovcExit != nullptr)
        vif.ovcExit(vif.module);
    if (handle != nullptr)
        ofsUnloadModule(handle);
    handle = nullptr;

    // Clear all registered module
    vif.module  = nullptr;
    vif.version = 0;
    vif.ovcInit = nullptr;
    vif.ovcExit = nullptr;
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
    cs = { 20, 20, 20 };

    mulat = 0.5;    // default lateral friction
    mulng = 0.1;    // default longitudinal friction

    // Object stands above 2 meters with 3 legs
    tdVertex_t tdvtx[3];
    tdvtx[0].pos = {  0, -2,  2 }; // forward leg
    tdvtx[1].pos = { -2, -2, -2 }; // left back leg
    tdvtx[2].pos = {  2, -2, -2 }; // right back leg
    for (int idx = 0; idx < ARRAY_SIZE(tdvtx); idx++)
    {
        tdvtx[idx].stiffness = 1e6;
        tdvtx[idx].damping = 1e5;
        tdvtx[idx].mulat = mulat;
        tdvtx[idx].mulng = mulng;
    }

    setTouchdownPoints(tdvtx, ARRAY_SIZE(tdvtx));

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

void Vehicle::setSurfaceFriction(double lat, double lng)
{
    mulat = lat;
    mulng = lng;
    for (auto &tp : tpVertices) {
        tp.mulat = mulat;
        tp.mulng = mulng;
    }
}

void Vehicle::initLanded(Object *object, double lat, double lng, double dir)
{

    // double slng = sin(lng), clng = cos(lng);
    // double slat = sin(lat), clat = cos(lat);
    // mat3d_t loc = { -slng,      0,     clng,
    //                  clat*clng, slat,  clat*slng,
    //                 -slat*clng, clat, -slat*slng };

    surface_t &sp = surfParam;

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

void Vehicle::initLanded(Celestial *object, const glm::dvec3 &loc, double dir)
{
    assert(object->getType() == objCelestialBody);
    CelestialPlanet *planet = dynamic_cast<CelestialPlanet *>(object);
    assert(planet != nullptr);

    ElevationManager *emgr = planet->getElevationManager();

    surface_t &sp = surfParam;

    sp.setLanded(loc, dir, object);

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

bool Vehicle::addSurfaceForces(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &s, double tfrac, double dt)
{
    // double alt = s.pos - cbody->getRadius();
    // if (alt > sp.elev + 1e4)
    //     return false;
    

    surface_t &sp = surfParam;
    StateVectors ps;
    sp.update(s, ps, cbody, &etile);
    double alt = sp.alt;
    // if (alt > 2.0*size)
    //     return false;

    CelestialPlanet *planet = dynamic_cast<CelestialPlanet *>(cbody);
    ElevationManager *emgr = planet->getElevationManager();

    // Check any touchdown points to touch ground
    double tdymin = std::numeric_limits<double>::infinity();
    int rlod = int(32.0 - log(std::max(sp.alt0, 0.1))*(1.0 / log(2.0)));
    glm::dmat3 T = glm::transpose(ps.R) * s.R;
    glm::dvec3 shift = glm::transpose(ps.R) * (s.pos - ps.pos);

    for (auto tp : tpVertices)
    {
        glm::dvec3 p = T * tp.pos + shift;
        double lat, lng, rad, elev = 0.0;
        cbody->convertLocalToEquatorial(p, lat, lng, rad);
        if (emgr != nullptr)
            elev = emgr->getElevationData({lat, lng, 0}, rlod, &etile);
        tp.tdy = rad - elev - cbody->getRadius();
        if (tp.tdy < tdymin)
            tdymin = tp.tdy;
    }

    // No, did not touch ground
    if (tdymin >= 0.0)
        return false;


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

void Vehicle::updateRadiationForces()
{

}

void Vehicle::updateBodyForces()
{
    surface_t &sp = surfParam;

    updateThrustForces();
    if (sp.isInAtomsphere) {
        // updateAerodynamicForces();
    }
}

void Vehicle::update(bool force)
{
    surface_t &sp = surfParam;

    if (fsType == fsFlight)
    {
        RigidBody::update(force);
    } 
    else if (fsType == fsLanded)
    {
        double period = cbody->getRotationPeriod();
        double velg = (period != 0.0) ? (cbody->getRadius() * sp.clat * pi2) / period : 0.0;
        s1.vel = { -velg * sp.slng, 0.0, velg * sp.clng};
        s1.R = sp.R;

        // {
        //     // Whenever the user engages engine,
        //     // switch to flight status
        //     fsType = fsFlight;

        //     s1.vel += irvel;
        //     brpos = s1.pos, irpos = {};
        //     brvel = s1.vel, irvel = {};
        //     s1.omega = {};
        // }
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

void Vehicle::updatePost()
{

}

void Vehicle::drawHUD(HUDPanel *hud, Sketchpad *pad)
{
    hud->drawDefault(pad);
}