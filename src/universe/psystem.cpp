// psystem.cpp - Planetary System package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#include "main/core.h"
#include "universe/celbody.h"
#include "universe/body.h"
#include "universe/star.h"
#include "universe/psystem.h"

pSystem::pSystem(CelestialStar *star)
: primaryStar(star)
{
    star->setpSystem(this);
    stars.push_back(star);
    bodies.push_back(star);
}

void pSystem::addStar(CelestialStar *star)
{
    if (!star->haspSystem())
        star->setpSystem(this);
    stars.push_back(star);
}

void pSystem::addBody(CelestialBody *cbody)
{
    bodies.push_back(cbody);
}

void pSystem::addGravity(CelestialBody *grav)
{
    gravities.push_back(grav);
}

void pSystem::addPlanet(CelestialBody *planet, CelestialBody *cbody)
{
    planets.push_back(planet);
    addBody(planet);
    addGravity(planet);
    planet->setStar(primaryStar);
    planet->setPlanetarySystem(this);
    planet->attach(cbody);
}

void pSystem::update(const TimeDate &td)
{

    for (auto star : stars)
        star->updateEphemeris(td);
    for (auto star : stars)
        star->updatePostEphemeris(td);   
}

void pSystem::finalizeUpdate()
{
    for (auto body : bodies)
        body->endUpdate();
}