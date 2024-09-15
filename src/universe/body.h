// body.h - Celestial body (Planets, Moons, Asteroids, etc) package
//
//  Author: Tim Stark
//  Date:   Sep 17, 2023

#pragma once

#include "universe/celbody.h"
#include "universe/elevmgr.h"
#include "universe/atmo.h"

class CelestialPlanet : public CelestialBody
{
public:
    CelestialPlanet(cstr_t &name, celType type);
    CelestialPlanet(YAML::Node &config, celType type);
    ~CelestialPlanet();

    void setup(YAML::Node &config);

    inline ElevationManager *getElevationManager() const    { return emgr; }

    inline bool hasAtmosphere() const           { return atm != nullptr; }
    inline Atmosphere *getAtmosphere() const    { return atm; }

    void getAtmParam(const glm::dvec3 &loc, atmprm_t *prm) const;

    inline double getSoundSpeed(double temp) const
        { return (atm != nullptr) ? sqrt(atmc.gamma * atmc.R * temp) : 0.0; }

private:
    ElevationManager *emgr = nullptr;

    Atmosphere *atm = nullptr;
    atmconst_t atmc;
};