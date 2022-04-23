// universe.h - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#include "universe/constellations.h"
#include "universe/starlib.h"
#include "universe/system.h"

class Universe
{
public:
    Universe() = default;
    ~Universe() = default;

    inline StarDatabase &getStarDatabase()      { return stardb; }
    inline Constellations &getConstellations()  { return constellations; }

    void init();

    System *createSolarSystem(celStar *star);
    System *getSolarSystem(celStar *star) const;

    celStar *findStar(cstr_t &name) const;

    Object *findObject(const Object *obj, const std::string &name) const;
    Object *findPath(cstr_t &path) const;

    int findCloseStars(const vec3d_t &obs, double mdist,
        std::vector<const celStar *> &closeStars) const;

private:
    StarDatabase stardb;
    Constellations constellations;

    SystemsList  systems;
};