// base.h - Spaceport/Airport package
//
// Author:  Tim Stark
// Date:    Oct 8, 2024

#pragma once

#include "engine/celestial.h"

class CelestialPlanet;

class Base : public Celestial
{
public:
    Base(json &config, CelestialPlanet *planet, double lat = 0, double lng = 0);
    virtual ~Base();

    void setup();
    void attach(CelestialPlanet *planet);

    void update(bool force) override;

protected:
    double lat, lng;
    double rad, elev;

    glm::dvec3 rpos;
    glm::dvec3 rvel;
    glm::dvec3 rotvel;
    glm::dmat3 rrot;
};