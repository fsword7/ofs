// celestial.cpp - Celestial Object package
//
// Author:  Tim Stark
// Date:    Aug 4, 2024

#include "main/core.h"
#include "api/celbody.h"
#include "ephem/elements.h"
#include "ephem/vsop87/vsop87.h"
#include "ephem/sol/luna/elp82.h"
#include "engine/celestial.h"
#include "universe/astro.h"
#include "utils/json.h"

Celestial::Celestial(cjson &config, ObjectType type, celType ctype)
: Object(config, type), cbType(ctype)
{
    setup(config);
    setupRotation();
}

Celestial::~Celestial()
{
    if (ephemeris != nullptr)
        delete ephemeris;
}

void Celestial::setup(cjson &config)
{

    if (getType() == objCelestialBody || getType() == objCelestialStar)
    {
        // Set up precession parameters
        eps_rel = myjson::getFloat<double>(config, "obliquity");
        precT   = myjson::getFloat<double>(config, "precession-period");
        eps_ref = myjson::getFloat<double>(config, "precession-obliquity");
        lan_ref = myjson::getFloat<double>(config, "precession-LAN");
        Lrel0   = myjson::getFloat<double>(config, "LAN");
        mjd_rel = myjson::getFloat<double>(config, "LAN-MJD", astro::MJD2000);

        // Set up rotation parameters
        Dphi = myjson::getFloat<double>(config, "sid-rot-offset");
        rotT = myjson::getFloat<double>(config, "sid-rot-period");

        Dphi += (ofsDate->getMJD0() - oel.getMJDEpoch() * ((86400.0/rotT)*pi2));
        Dphi += Lrel0 * cos(eps_rel);
        Dphi = fmod(Dphi, pi2);

        str_t epName = myjson::getString<str_t>(config, "orbit");
        if (!epName.empty())
        {
            OrbitEphemeris *orbit = OrbitVSOP87::create(*this, epName);
            if (orbit == nullptr)
                orbit = OrbitELP82::create(*this, epName);
            if (orbit != nullptr)
                ephemeris = orbit;
            else
                ofsLogger->error("OFS: Unknown orbital ephemeris: {}\n", epName);
        }

        reflectivity = myjson::getFloat<double>(config, "geom-albedo");
        geomColor = myjson::getFloatArray<color_t, float>(config, "color", { 0.5, 0.5, 0.5, 1.0 });
    }
}

void Celestial::setupRotation()
{
    Rref = glm::dmat3();
    if (eps_ref)
    {
        glm::dmat3 Lref;
        double sine = sin(eps_ref), cose = cos(eps_ref);
        double sinl = sin(lan_ref), cosl = cos(lan_ref);
        Rref = { 1, 0, 0,  0,cose,-sine,  0,sine, cose };
        Lref = { cosl,0,sinl,  0,1,0,  -sinl,0,cosl };
        Rref = Lref * Rref;
    }

    precOmega = (precT ? pi2/precT : 0.0);
    rotOmega = pi2/rotT;
    cos_eps = cos(eps_rel), sin_eps = sin(eps_rel);

    updatePrecission();
    updateRotation();
}

void Celestial::attach(Celestial *parent, frameType type)
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

StateVectors Celestial::interpolateState(double step)
{
    if (step == 0.0)
        return s0;
    if (step == 1.0)
        return s1;

    StateVectors sv;
    sv.pos   = interpolatePosition(step);
    sv.vel   = (s0.vel*(1.0-step)) + (s1.vel*step);
    //sv.R = getRotation(ofsDate.getSimTime0() + ofsDate.getDeltaTime()*step);
    sv.Q     = sv.R;
    sv.omega = (s0.omega*(1.0-step)) + (s1.omega*step);

    return sv;
}

glm::dvec3 Celestial::interpolatePosition(double n) const
{
    if (n == 0.0)
        return s0.pos;
    else if (n == 1.0)
        return s1.pos;
    
    const double eps = 1e-2;
    glm::dvec3 refp0(0, 0, 0);
    glm::dvec3 refp1(0, 0, 0);
    glm::dvec3 refpm(0, 0, 0);

    Celestial *cbody = nullptr; // getReferenceOrbit();
    if (cbody != nullptr)
    {
        refp0 = cbody->s0.pos;
        refp1 = cbody->s1.pos;
        refpm = cbody->interpolatePosition(n);
    }

    glm::dvec3 rp0 = s0.pos - refp0;
    glm::dvec3 rp1 = s1.pos - refp1;
    double rd0 = glm::length(rp0);
    double rd1 = glm::length(rp1);
    double n0 = 0.0;
    double n1 = 1.0;
    double nm = 0.5, d = 0.5;
    double rdm = (rd0 + rd1) * 0.5;
    glm::dvec3 rpm = glm::normalize(rp0 + rp1) * rdm;

    while (fabs(nm - n) > eps && d > eps)
    {
        d *= 0.5;
        if (nm < n) {
            rp0 = rpm, rd0 = rdm;
            n0 = nm, nm += d;
        } else {
            rp1 = rpm, rd1 = rdm;
            n1 = nm, nm -= d;
        }
        rdm = (rd0 + rd1) * 0.5;
        rpm = glm::normalize(rp0 + rp1) * rdm;
    }

    if (fabs(nm - n) > 1e-10)
    {
        double scale = (n - n0) / (n1 - n0);
        rdm = rd0 + (rd1 - rd0)*scale;
        rpm = glm::normalize(rp0 + (rp1 - rp0)*scale) * rdm;
    }

    return rpm + refpm;
}

void Celestial::updatePrecission()
{
    Lrel = Lrel0 + precOmega * (ofsDate->getMJD1() - mjd_rel);
    double sinl = sin(Lrel), cosl = cos(Lrel);

    glm::dmat3 Rrel = { cosl,   -sinl*sin_eps,      -sinl*cos_eps, 
                        0,      cos_eps,            -sin_eps,
                        sinl,   cosl*sin_eps,        cosl*cos_eps };

    if (eps_ref)
        Rrel = Rref * Rrel;
    
    Raxis = Rrel * glm::dvec3( 0, 1, 0 );
    eps_ecl = acos(Raxis.y);
    lan_ecl = atan2(-Raxis.x, Raxis.z);

    double sinL = sin(lan_ecl), cosL = cos(lan_ecl);
    double sine = sin(eps_ecl), cose = cos(eps_ecl);

    Recl = { cosL,  -sinL*sine,  -sinL*cose,
             0,      cose,       -sine,
             sinL,   cosL*sine,  cosL*cose };
    
    double cos_poff = cosL*Rrel[0][0] * sinL*Rrel[2][0];
    double sin_poff = -(cosL*Rrel[0][2] * sinL*Rrel[2][2]);
    rotofs = atan2(sin_poff, cos_poff);
}

glm::dmat3 Celestial::getRotation(double t) const
{
    glm::dmat3 rot;

    double r = ofs::posangle(Dphi + rotOmega*t - Lrel*cos_eps + rotofs);
    double cosr = cos(r), sinr = sin(r);
    rot = {  cosr, 0.0, sinr,
             0.0,  1.0, 0.0,
            -sinr, 0.0, cosr };
    return rot * Recl;
}

void Celestial::updateRotation()
{
    crot = ofs::posangle(Dphi + ofsDate->getSimTime1()*rotOmega - Lrel*cos_eps + rotofs);

    // if (getsName() == "Earth")
    //     ofsLogger->info("Time: {} -> rot {} ({} degreees)\n",
    //         ofsDate->getSimTime1(), crot, glm::degrees(crot));

    double cosr = cos(crot), sinr = sin(crot);
    glm::dmat3 rot;

    rot = {  cosr, 0.0, sinr,
             0.0,  1.0, 0.0,
            -sinr, 0.0, cosr };
    s1.R = rot * Recl;
    // s1.R = rot;
    s1.Q = s1.R;

    objRotation  = s1.R;
    objqRotation = s1.Q;
}

void Celestial::updateCelestial(bool force)
{
    if (precT != 0.0)
        updatePrecission();
    updateRotation();

    // RigidBody::update(force);
}

void Celestial::convertPolarToXYZ(double *pol, double *xyz, bool hpos, bool hvel)
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

uint32_t Celestial::getEphemerisState(double *res)
{
    uint16_t req = EPHEM_TRUEPOS|EPHEM_TRUEVEL|EPHEM_BARYPOS|EPHEM_BARYVEL;

    if (ephemeris == nullptr)
        return 0;
    return ephemeris->getOrbitData(ofsDate->getMJD1(), req, res);
}

bool Celestial::updateEphemeris()
{
    double state[12];
    int flags;
    bool htrue = false;
    bool hbary = false;
    bool hvel = false;
 
    // ofsLogger->info("Updating: {}  Time: {}\n", getsName(), ofsDate->getMJD1());

    // Updating secondaries (planets/moons) recursively
    // from origin of star or specific body
    for (auto body : secondaries)
        body->updateEphemeris();

    if (flags = getEphemerisState(state))
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

        // ofsLogger->info("{}: P({:.6f},{:.6f},{:.6f}) V({:.6f},{:.6f},{:.6f})\n", getsName(),
        //     cpos.x, cpos.y, cpos.z, cvel.x, cvel.y, cvel.z);
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

    // ofsLogger->info("{}: P({:.6f},{:.6f},{:.6f}) V({:.6f},{:.6f},{:.6f})\n", getsName(),
    //     cpos.x, cpos.y, cpos.z, cvel.x, cvel.y, cvel.z);

    bparent = (flags & EPHEM_PARENT) != 0;

    return bparent;
}

void Celestial::updatePostEphemeris()
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

    // ofsLogger->info("{}: P({:.6f},{:.6f},{:.6f}) V({:.6f},{:.6f},{:.6f})\n", getsName(),
    //     objPosition.x, objPosition.y, objPosition.z,
    //     objVelocity.x, objVelocity.y, objVelocity.z);

    // Updating secondaries recursively
    for (auto body : secondaries)
        body->updatePostEphemeris();
}
