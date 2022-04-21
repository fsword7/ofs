// universe.h - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#include "universe/starlib.h"
#include "universe/system.h"

class Universe
{
public:
    Universe() = default;
    ~Universe() = default;

    inline StarDatabase &getStarDatabase() { return stardb; }
    
    void init();

    System *createSolarSystem(celStar *star);
    System *getSolarSystem(celStar *star) const;

    celStar *findStar(cstr_t &name);

    int findCloseStars(const vec3d_t &obs, double mdist,
        std::vector<const celStar *> &closeStars) const;

private:
    StarDatabase stardb;

    SystemsList  systems;
};