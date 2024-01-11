// celbody.cpp - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "api/celbody.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "ephem/vsop87/vsop87.h"
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

CelestialBody::CelestialBody(json &cfg, celType type)
: RigidBody(cfg, (type == cbStar) ? objCelestialStar : objCelestialBody),
  cbType(type)
{

    // getValueReal(cfg, "LAN", Lrel0);
    // getValueReal(cfg, "LAN_MJD", mjd_rel);
    // getValueReal(cfg, "SidRotPeriod", rotT);
    // getValueReal(cfg, "SidRotOffset", Dphi);
    // getValueReal(cfg, "Obliquity", eps_rel);

    // getValueReal(cfg, "PrecessionPeriod", precT);
    // getValueReal(cfg, "PrecessionObliquity", eps_ref);
    // getValueReal(cfg, "PreccesionLAN", lan_ref);

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

void CelestialBody::convertPolarToXYZ(double *pol, double *xyz, bool hpos, bool hvel)
{
    // double cosp = cos(pol[0]), sinp = sin(pol[0]); // Phi
    double cosp = cos(pol[0]), sinp = -sin(pol[0]); // Phi
    double cost = cos(pol[1]), sint = sin(pol[1]); // Theta
    double rad  = pol[2] * KM_PER_AU;
    double xz   = rad * cost;

    if (hpos)
    {
        xyz[0] = xz * cosp;
        xyz[2] = xz * sinp;
        xyz[1] = rad * sint;
    }

    if (hvel)
    {
        double vl = xz * pol[3];
        double vb = rad * pol[4];
        double vr = pol[5] * KM_PER_AU;

        xyz[3] = cosp*cost*vr - cosp*sint*vb - sinp*vl;
        xyz[4] = sint*vr      + cost*vb;
        xyz[5] = sinp*cost*vr - sinp*sint*vb + cosp*vl;
    }
}

uint32_t CelestialBody::getEphemerisState(const TimeDate &td, double *res)
{
    uint16_t req = EPHEM_TRUEPOS|EPHEM_TRUEVEL|EPHEM_BARYPOS|EPHEM_BARYVEL;

    if (ephemeris == nullptr)
        return 0;
    return ephemeris->getOrbitData(td.getMJD1(), req, res);
}

bool CelestialBody::updateEphemeris(const TimeDate &td)
{
    double state[12];
    int flags;
    bool htrue = false;
    bool hbary = false;
    bool hvel = false;
 
    // Updating secondaries (planets/moons) recursively
    // from origin of star or specific body
    for (auto body : secondaries)
        body->updateEphemeris(td);

    if (flags = getEphemerisState(td, state))
    {
        if (flags & EPHEM_TRUEBARY)
        {
            // For celestial body with no planetary system

            // Get orbital or barycentric position/velocity either
            double *s;
            if (flags & EPHEM_TRUEPOS)
            {
                s = state;
                if (flags & EPHEM_TRUEVEL)
                    hvel = true;
            }
            else if (flags & EPHEM_BARYPOS)
            {
                s = state+6;
                if (flags & EPHEM_BARYVEL)
                    hvel = true;
            }

            if (flags & EPHEM_POLAR)
                convertPolarToXYZ(s, s, true, hvel);
            htrue = hbary = true;
            cpos = bpos = { s[0], s[1], s[2] };
            if (hvel == true)
                cvel = bvel = { s[3], s[4], s[5] };
        }
        else
        {
            // For celestial body with planetary system

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
    }
    else
    {
        // Updating orbital elements
        oel.update(0, bpos, bvel);
        hbary = true;
    }

    // Updating barycentre positions and velocity through reference frame.
    if (hbary && htrue)
    {
        // With known orbital and barycentirc position/velocity,
        // just calculate difference between them. 
        bposofs = bpos - cpos;
        bvelofs = bvel - cvel;
    }
    else
    {
        // Celculate wobbling position/velocity
        // by using mass of celestial body for new
        // orbital or barycentric position/velocity
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

void CelestialBody::updatePostEphemeris(const TimeDate &td)
{
    s1.pos = cpos;
    s1.vel = cvel;
    if (cbody != nullptr)
    {
        // Relative to origin of barycentre (star)
        s1.pos += cbody->s1.pos;
        s1.vel += cbody->s1.vel;
    }
    objPosition = s1.pos;
    objVelocity = s1.vel;
    baryPosition = bpos;
    baryVelocity = bvel;

    // Logger::getLogger()->info("{}: P({:.6f},{:.6f},{:.6f}) V({:.6f},{:.6f},{:.6f})\n", getsName(),
    //     objPosition.x, objPosition.y, objPosition.z,
    //     objVelocity.x, objVelocity.y, objVelocity.z);

    // Updating secondaries recursively
    for (auto body : secondaries)
        body->updatePostEphemeris(td);
}

void CelestialBody::updatePrecission(const TimeDate &td)
{
    Lrel = Lrel0 + precOmega * (td.getMJD1() - mjd_rel);
    double sinl = sin(Lrel), cosl = cos(Lrel);

    glm::dmat3 Rrel = { cosl,   -sinl*sin_eps,      -sinl*cos_eps, 
                        0,      cos_eps,            -sin_eps,
                        sinl,   cosl*sin_eps,       cosl*cos_eps };

    if (eps_ref)
        R_ref_rel = R_ref * R_ref_rel;
    
    Raxis = R_ref_rel * glm::dvec3( 0, 1, 0 );
    eps_ecl = acos(Raxis.y);
    lan_ecl = atan2(-Raxis.y, Raxis.z);

    double sinL = sin(lan_ecl), cosL = cos(lan_ecl);
    double sine = sin(eps_ecl), cose = cos(eps_ecl);

    Recl = { cosL,  -sinL*sine, -sinL*cose,
            0,      cose,       -sine,
            sinL,   cosL*sine,  cosL*cose };
    
    double cos_poff = cosL*R_ref_rel[0][0] * sinL*R_ref_rel[2][0];
    double sin_poff = -(cosL*R_ref_rel[0][2] * sinL*R_ref_rel[2][2]);
    rotOffset = atan2(sin_poff, cos_poff);
}

void CelestialBody::updateRotation(const TimeDate &td)
{
    crot = ofs::posangle(Dphi + td.getSimTime1()*rotOmega - Lrel*cos_eps + rotofs);

    double cosr = cos(crot), sinr = sin(crot);
    s1.R = {  cosr, 0.0, sinr,
              0.0,  1.0, 0.0,
             -sinr, 0.0, cosr };

    s1.Q = s1.R;
}

void CelestialBody::update(const TimeDate &td, bool force)
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

bool CelestialBody::load(json &cfg)
{

    // getValueReal(cfg, "Mass", mass);
    // getValueReal(cfg, "Radius", radius);

    // getValueReal(cfg, "LAN", Lrel0);
    // getValueReal(cfg, "LAN_MJD", mjd_rel);
    // getValueReal(cfg, "SidRotPeriod", rotT);
    // getValueReal(cfg, "SidRotOffset", Dphi);
    // getValueReal(cfg, "Obliquity", eps_rel);

    // getValueReal(cfg, "PrecessionPeriod", precT);
    // getValueReal(cfg, "PrecessionObliquity", eps_ref);
    // getValueReal(cfg, "PreccesionLAN", lan_ref);
    
    return true;
}