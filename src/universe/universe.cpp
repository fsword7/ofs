// universe.cpp - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/astro.h"

void Universe::init()
{
    stardb.loadXHIPData("xhip");
}

System *Universe::createSolarSystem(celStar *star)
{
    System *system = getSolarSystem(star);
    if (system != nullptr)
        return system;

    auto idStar = star->getHIPnumber();
    system = new System(star);
    systems.insert({idStar, system});

    return system;
}

System *Universe::getSolarSystem(celStar *star) const
{
    if (star == nullptr)
        return nullptr;
    auto idStar = star->getHIPnumber();
    auto iter = systems.find(idStar);
    if (iter != systems.end())
        return iter->second;
    return nullptr;
}

celStar *Universe::findStar(cstr_t &name)
{
    return stardb.find(name);
}

int Universe::findCloseStars(const vec3d_t &obs, double mdist,
    std::vector<const celStar *> &closeStars) const
{
    return stardb.findCloseStars(obs, mdist, closeStars);
}
