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
{
    star->setpSystem(this);
    stars.push_back(star);
    bodies.push_back(star);
}

void pSystem::addBody(CelestialBody *cbody)
{
    bodies.push_back(cbody);
}

void pSystem::addStar(CelestialStar *star)
{
    if (!star->haspSystem())
        star->setpSystem(this);
    stars.push_back(star);
}

void pSystem::update(const TimeDate &td)
{

    for (auto star : stars)
        star->updateEphemeris(td);
    for (auto star : stars)
        star->updatePostEphemeris(td);   
}