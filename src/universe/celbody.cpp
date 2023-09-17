// celbody.cpp - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "api/celbody.h"
#include "engine/rigidbody.h"
#include "ephem/vsop87a.h"
#include "ephem/rotation.h"
#include "universe/celbody.h"
#include "universe/astro.h"

// ******** Planetary System ********

PlanetarySystem::PlanetarySystem(CelestialBody *body)
: body(body), tree(body)
{
    bodies.clear();
}

PlanetarySystem::PlanetarySystem(CelestialStar *star)
: star(star), tree(star)
{
    bodies.clear();
}

void PlanetarySystem::addBody(CelestialBody *body)
{
    bodies.push_back(body);
    body->setInSystem(this);
}

void PlanetarySystem::removeBody(CelestialBody *body)
{
}

CelestialBody *PlanetarySystem::find(cstr_t &name) const
{
    for (auto body : bodies)
    {
        if (name == body->getsName())
            return body;
    }
    return nullptr;
}

// ******** Celestial Body ********

PlanetarySystem *CelestialBody::createPlanetarySystem()
{
    if (ownSystem != nullptr)
        return ownSystem;

    ownSystem = new PlanetarySystem(this);
    ownSystem->setStar(inSystem->getStar());
    return ownSystem;
}

void CelestialBody::convertPolarToXYZ(double *pol, double *xyz, bool hpos, bool hvel)
{
    double cosp = cos(pol[0]), sinp = sin(pol[0]); // Phi
    double cost = cos(pol[1]), sint = sin(pol[1]); // Theta
    double rad  = pol[2] * KM_PER_AU;
    double xz   = rad * cost;

    if (hpos)
    {
        xyz[0] = xz * cosp;
        xyz[1] = xz * sinp;
        xyz[2] = rad * sint;
    }

    if (hvel)
    {
        double vl = xz * pol[3];
        double vb = rad * pol[4];
        double vr = pol[5] * KM_PER_AU;

        xyz[3] = cosp*cost*vr - cosp*sint*vb - sinp*vl;
        xyz[4] = sint*vr      + cost*vb;
        xyz[5] = sinp*cost*vr - sinp*sint*vb * cosp*vl;
    }
}

uint32_t CelestialBody::getEphemerisState(double *res)
{
    uint16_t req = EPHEM_TRUEPOS|EPHEM_TRUEVEL|EPHEM_BARYPOS|EPHEM_BARYVEL;

    if (ephemeris == nullptr)
        return 0;
    return ephemeris->getOrbitData(0, req, res);
}

bool CelestialBody::updateEphemeris()
{
    double state[12];
    int flags;
    bool htrue = false;
    bool hbary = false;
    bool hvel = false;
 
    // Updating secondaries (planets/moons) recursively
    // from origin of star or specific body
    for (auto body : secondaries)
        body->updateEphemeris();

    if (flags = getEphemerisState(state))
    {

        // Orbital position/velocity
        if (flags & EPHEM_TRUEPOS)
        {
            htrue = true;
            hvel = (flags & EPHEM_TRUEVEL) != 0;
            if (flags & EPHEM_POLAR)
                convertPolarToXYZ(state, state, true, hvel);
            cpos = { state[0], state[1], state[2] };
            if (flags & EPHEM_TRUEVEL)
                cvel = { state[3], state[4], state[5] };
        }

        // Barycentre position/velocity
        if (flags & EPHEM_BARYPOS)
        {
            hbary = true;
            hvel = (flags & EPHEM_BARYVEL) != 0;
            if (flags & EPHEM_POLAR)
                convertPolarToXYZ(state+6, state+6, true, hvel);
            bpos = { state[6], state[7], state[8] };
            if (flags & EPHEM_BARYVEL)
                bvel = { state[9], state[10], state[11] };
        }
    }
    else
    {
        // Updating orbital elements

    }

    // Updating barycentre positions and velocity through reference frame.
    if (hbary && htrue)
    {
        bposofs = bpos - cpos;
        bvelofs = bvel - cvel;
    }
    else
    {
        bposofs = { 0, 0, 0 };
        bvelofs = { 0, 0, 0 };
        double bmass = mass;
        for (auto body : secondaries)
        {
            bposofs += body->bpos * body->getMass();
            bvelofs += body->bvel * body->getMass();
            if (!body->bparent)
                bmass += body->getMass();
        }
        bposofs /= bmass;
        bvelofs /= bmass;

        if (htrue)
        {
            bpos = cpos + bposofs;
            bvel = cvel + bvelofs;
        }
        else
        {
            cpos = bpos - bposofs;
            cvel = bvel - bvelofs;
        }
    }

    for (auto body : secondaries)
    {
        if (body->bparent)
        {
            body->cpos += bposofs;
            body->cvel += bvelofs;
            body->bpos += bposofs;
            body->bvel += bvelofs;
        }
    }

    bparent = (flags & EPHEM_PARENT) != 0;

    return bparent;
}

void CelestialBody::update(bool force)
{

    RigidBody::update(force);
}

glm::dmat3 CelestialBody::getEquatorial(double tjd) const
{
    return glm::dmat3(1.0);
}

double CelestialBody::getLuminosity(double lum, double dist) const
{
    double power = lum * SOLAR_POWER;
    double irradiance = power / ofs::sphereArea(dist * 1000.0);
    double incidentEnergy = irradiance * ofs::circleArea(radius * 1000.0);
    double reflectedEnergy = incidentEnergy * albedo;

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
    return (rotation != nullptr ? rotation->getRotation(tjd) : glm::dmat3(1.0)) * bodyFrame->getOrientation(tjd);
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