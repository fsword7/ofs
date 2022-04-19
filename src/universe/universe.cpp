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
    stardb.load();
}

celStar *Universe::findStar(cstr_t &name)
{
    return stardb.find(name);
}

int Universe::findCloseStars(const vec3d_t &obs, double mdist,
    std::vector<const celStar *> &closeStars) const
{
    return stardb.findCloseStars(obs / KM_PER_PC, mdist, closeStars);
}
