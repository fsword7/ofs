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
