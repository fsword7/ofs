// starlib.cpp - Star database library package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/star.h"
#include "universe/starlib.h"
#include "universe/universe.h"

void StarDatabase::load()
{
    celStar *sun = celStar::createTheSun();
    uStars.push_back(sun);
}

celStar *StarDatabase::find(cstr_t &name)
{
    if (uStars.size() > 0)
        return uStars[0];
    return nullptr;
}

int StarDatabase::findCloseStars(const vec3d_t &obs, double radius,
    std::vector<const celStar *> &stars) const
{
    if (uStars.size() > 0)
        stars.push_back(uStars[0]);
    return stars.size();
}
