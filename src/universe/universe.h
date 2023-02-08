// universe.h - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#include "universe/constellations.h"
#include "universe/starlib.h"
#include "universe/system.h"
#include "universe/handle.h"

class OFSAPI Universe
{
public:
    Universe() = default;
    ~Universe() = default;

    inline StarDatabase &getStarDatabase()      { return stardb; }
    inline Constellations &getConstellations()  { return constellations; }

    void init();

    System *createSolarSystem(CelestialStar *star);
    System *getSolarSystem(CelestialStar *star) const;

    CelestialStar *findStar(cstr_t &name) const;

    Object *findObject(const Object *obj, const std::string &name) const;
    Object *findPath(cstr_t &path) const;

    // int findCloseStars(const vec3d_t &obs, double mdist,
    //     std::vector<const celStar *> &closeStars) const;
    int findCloseStars(const glm::dvec3 &obs, double mdist,
        std::vector<const CelestialStar *> &closeStars) const;

    void findVisibleStars(ofsHandler &handler,
        const glm::dvec3 &obs, const glm::dmat3 &rot,
        const double fov, const double aspect,
        const double faintest);

    Object *pickPlanet(System *system, const glm::dvec3 &obs, const glm::dvec3 &dir, double when);
    Object *pick(const glm::dvec3 &obs, const glm::dvec3 &dir, double when);

private:
    StarDatabase stardb;
    Constellations constellations;

    SystemsList  systems;
};