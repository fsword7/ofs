// base.cpp - Spaceport/Airport package
//
// Author:  Tim Stark
// Date:    Oct 8, 2024

#include "main/core.h"
#include "universe/body.h"
#include "engine/base.h"

Base::Base(json &config, CelestialPlanet *planet, double lat, double lng)
: Celestial(config, objSurfBase, cbSurfBase), lat(lat), lng(lng)
{
    // Assign this base to planet as target
    // relative/horizon reference frame.
    attach(planet);
}

Base::~Base()
{

}

void Base::setup()
{
    CelestialPlanet *planet = dynamic_cast<CelestialPlanet *>(cbody);

    assert(planet != nullptr);
    ElevationManager *emgr = planet->getElevationManager();

    elev = emgr->getElevationData({lat, lng, rad});
    if (elev != 0.0)
        rad += elev;
    rpos = cbody->convertEquatorialToLocal(lat, lng, rad);
}

void Base::attach(CelestialPlanet *planet)
{
    cbody = planet;

    rad = cbody->getRadius();
    elev = 0.0;
    s0.vel = { 0, 0, 0 }; // always fixed to ground

    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0).
    //
    //     |  slat  clat   0  | |  clng   0   slng |
    // R = | -clat  slat   0  | |   0     1    0   |
    //     |   0     0     1  | | -slng   0   clng |
    //
    // Apply 90 degrees counterclockwise rotation to point north.
    double clat = cos(lat), slat = sin(lat);
    double clng = cos(lng), slng = sin(lng);
    rrot = { slat*clng,  clat*clng, slng,
            -clat,       slat,      0,
            -slat*slng, -clat*slng, clng };

    double v = (pi2 * rad * clat) / planet->getRotationPeriod();
    rotvel = { -v*slng, 0, v*clng };
}

void Base::update(bool force)
{
    s1.pos = (rpos * cbody->s1.R) + cbody->s1.pos;
    s1.vel = (rotvel * cbody->s1.R) + cbody->s1.vel;
    s1.R = rrot * cbody->s1.R;
    s1.Q = s1.R;
}