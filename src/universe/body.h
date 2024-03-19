// body.h - Celestial body (Planets, Moons, Asteroids, etc) package
//
//  Author: Tim Stark
//  Date:   Sep 17, 2023

#pragma once

#include "universe/celbody.h"
#include "universe/elevmgr.h"

class CelestialPlanet : public CelestialBody
{
public:
    CelestialPlanet(cstr_t &name, celType type);
    CelestialPlanet(YAML::Node &config, celType type);
    ~CelestialPlanet();

    inline ElevationManager *getElevationManager() const    { return emgr; }

private:
    ElevationManager *emgr = nullptr;
};