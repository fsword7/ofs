// body.cpp - Celestial body (Planets, Moons, Asteroids, etc) package
//
//  Author: Tim Stark
//  Date:   Sep 17, 2023

#include "main/core.h"
#include "universe/body.h"

CelestialPlanet::CelestialPlanet(cstr_t &name, celType type)
: CelestialBody(name, type)
{
    emgr = new ElevationManager(this);
}

CelestialPlanet::CelestialPlanet(YAML::Node &config, celType type)
: CelestialBody(config, type)
{
    emgr = new ElevationManager(this);   
}

CelestialPlanet::~CelestialPlanet()
{
    if (emgr != nullptr)
        delete emgr;
}