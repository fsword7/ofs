// celbody.cpp - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "api/celbody.h"
#include "api/ofsapi.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "ephem/vsop87/vsop87.h"
#include "ephem/lunar/elp82.h"
#include "ephem/rotation.h"
// #include "ephem/elements.h"
#include "universe/elevmgr.h"
#include "universe/surfmgr.h"
#include "universe/celbody.h"
#include "universe/astro.h"

// ******** Planetary System ********

// PlanetarySystem::PlanetarySystem(CelestialBody *body)
// : body(body), tree(body)
// {
//     bodies.clear();
// }

// PlanetarySystem::PlanetarySystem(CelestialStar *star)
// : star(star), tree(star)
// {
//     bodies.clear();
// }

// void PlanetarySystem::addBody(CelestialBody *body)
// {
//     bodies.push_back(body);
//     body->setInSystem(this);
// }

// void PlanetarySystem::removeBody(CelestialBody *body)
// {
// }

// CelestialBody *PlanetarySystem::find(cstr_t &name) const
// {
//     for (auto body : bodies)
//     {
//         if (name == body->getsName())
//             return body;
//     }
//     return nullptr;
// }

// ******** Celestial Body ********

// PlanetarySystem *CelestialBody::createPlanetarySystem()
// {
//     if (ownSystem != nullptr)
//         return ownSystem;

//     ownSystem = new PlanetarySystem(this);
//     ownSystem->setStar(inSystem->getStar());
//     return ownSystem;
// }

CelestialBody::CelestialBody(YAML::Node &config, ObjectType type, celType cbtype)
: RigidBody(config, type, cbtype)
{

    // if (config["Orbit"].IsScalar())
    // {
    //     str_t epName = config["Orbit"].as<str_t>();
    //     OrbitEphemeris *orbit = OrbitVSOP87::create(*this, epName);
    //     if (orbit == nullptr)
    //         orbit = OrbitELP82::create(*this, epName);

    //     if (orbit != nullptr)
    //         setEphemeris(orbit);
    //     else
    //         ofsLogger->error("OFS Error: Unknown orbital ephemeris: {}\n", epName);
    // }

    // getValueReal(cfg, "LAN", Lrel0);
    // getValueReal(cfg, "LAN_MJD", mjd_rel);
    // getValueReal(cfg, "SidRotPeriod", rotT);
    // getValueReal(cfg, "SidRotOffset", Dphi);
    // getValueReal(cfg, "Obliquity", eps_rel);

    // getValueReal(cfg, "PrecessionPeriod", precT);
    // getValueReal(cfg, "PrecessionObliquity", eps_ref);
    // getValueReal(cfg, "PreccesionLAN", lan_ref);

}

CelestialBody::~CelestialBody()
{
    if (ephemeris != nullptr)
        delete ephemeris;
}

void CelestialBody::attach(CelestialBody *parent, frameType type)
{
    assert(parent != nullptr);

    if (parent != cbody)
    {
        cbody = parent;
        parent->addSecondary(this);

        // elements->setup(mass, cbody->getMass(), 0); // td.mjdRef);

        uint16_t flags = 1;
        if (flags)
        {
            // if (flags & EPHEM_TRUEPOS)
            //     s0->pos = bpos;
            // if (flags & EPHEM_TRUEVEL)
            //     s0->vel = bvel;
            // if (flags & EPHEM_BARYPOS)
            //     bpos = s0->pos;
            // if (flags & EPHEM_BARYVEL)
            //     bvel = s0->vel;
        }
        else
        {
            oel.getPositionVelocity(0, s0.pos, s0.vel);
            // s0.pos = cbody->Recl * s0.pos;
            // s0.vel = cbody->Recl * s0.vel;
            // oel.calculate(0, s0.pos, s0.vel);

            bpos = s0.pos;
            bvel = s0.vel;
        }

        // Relative to parent celestial body
        s0.pos += cbody->s0.pos;
        s0.vel += cbody->s0.vel;
        bpos += cbody->s0.pos;
        bvel += cbody->s0.vel;

        // On the air
        objPosition = s0.pos;
        objVelocity = s0.vel;
    }
}

// void CelestialBody::update(const TimeDate &td, bool force)
// {

//     updatePrecission();
//     updateRotation();

//     RigidBody::update(force);
// }

glm::dmat3 CelestialBody::getEquatorial(double tjd) const
{
    return glm::dmat3(1.0);
}

double CelestialBody::getLuminosity(double lum, double dist) const
{
    double power = lum * SOLAR_POWER;
    double irradiance = power / ofs::sphereArea(dist * 1000.0);
    double incidentEnergy = irradiance * ofs::circleArea(radius * 1000.0);
    double reflectedEnergy = incidentEnergy * geomAlbedo;

    return reflectedEnergy / SOLAR_POWER;
}

double CelestialBody::getApparentMagnitude(glm::dvec3 sun, double irradiance, glm::dvec3 view) const
{
    double vdist  = glm::length(view);
    double sdist  = glm::length(sun);
    double illum  = (1.0 * glm::dot(sun / sdist, sun / sdist)) / 2.0;
    double absMag = getLuminosity(irradiance, sdist) * illum;

    return astro::convertLumToAppMag(absMag, astro::convertKilometerToParsec(vdist));
}

glm::dmat3 CelestialBody::getBodyFixedFromEcliptic(double tjd) const
{
    // return (rotation != nullptr ? rotation->getRotation(tjd) : glm::dmat3(1.0)) * bodyFrame->getOrientation(tjd);
    return glm::dmat3(1.0);
}

glm::dvec3 CelestialBody::getPlanetocentric(glm::dvec3 pos) const
{
    glm::dvec3 w = glm::normalize(pos);

    double lat = acos(w.y) - (pi / 2.0);
    double lng = atan2(w.z, -w.x);

    return glm::dvec3(lat, lng, glm::length(pos) - radius);
}

glm::dvec3 CelestialBody::getPlanetocentricFromEcliptic(const glm::dvec3 &pos, double tjd) const
{
    glm::dvec3 lpos = getBodyFixedFromEcliptic(tjd) * pos;

    return getPlanetocentric(lpos);
}

glm::dvec3 CelestialBody::getvPlanetocentricFromEcliptic(const glm::dvec3 &pos, double tjd) const
{
    return getBodyFixedFromEcliptic(tjd) * pos;
}

glm::dvec3 CelestialBody::getHeliocentric(double tjd) const
{
    glm::dvec3 opos = getuPosition(tjd);
    glm::dvec3 hpos = glm::normalize(opos);

    double lat = acos(hpos.y) - (pi / 2.0);
    double lng = atan2(hpos.z, -hpos.x);

    return glm::dvec3( lng, lat, glm::length(opos));
}

// bool CelestialBody::load(json &cfg)
// {

//     // getValueReal(cfg, "Mass", mass);
//     // getValueReal(cfg, "Radius", radius);

//     // getValueReal(cfg, "LAN", Lrel0);
//     // getValueReal(cfg, "LAN_MJD", mjd_rel);
//     // getValueReal(cfg, "SidRotPeriod", rotT);
//     // getValueReal(cfg, "SidRotOffset", Dphi);
//     // getValueReal(cfg, "Obliquity", eps_rel);

//     // getValueReal(cfg, "PrecessionPeriod", precT);
//     // getValueReal(cfg, "PrecessionObliquity", eps_ref);
//     // getValueReal(cfg, "PreccesionLAN", lan_ref);
    
//     return true;
// }