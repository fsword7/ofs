// universe.h - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#include "universe/constellations.h"
#include "universe/starlib.h"
#include "universe/psystem.h"
#include "universe/handle.h"
#include "universe/celbody.h"

class Player;
class Vehicle;
class Celestial;
class CelestialPlanet;

class OFSAPI Universe
{
public:
    Universe() = default;
    ~Universe() = default;

    inline StarDatabase &getStarDatabase()      { return stardb; }
    inline Constellations &getConstellations()  { return constellations; }
    inline std::vector<const CelestialStar *> &getNearStars() { return nearStars; }

    void init();
    void start();
    void configure(cjson &config);
    void update(Player *player, const TimeDate &td);
    void finalizeUpdate();

    pSystem *createSolarSystem(cstr_t &sysName);
    pSystem *getSolarSystem(cstr_t &sysName) const;
    void addSystem(pSystem *psys);

    CelestialStar *findStar(cstr_t &name) const;

    Celestial *findObject(const Object *obj, const std::string &name) const;
    Celestial *findPath(cstr_t &path) const;

    // int findCloseStars(const vec3d_t &obs, double mdist,
    //     std::vector<const celStar *> &closeStars) const;
    int findCloseStars(const glm::dvec3 &obs, double mdist,
        std::vector<const CelestialStar *> &closeStars) const;

    void findVisibleStars(ofsHandler &handler,
        const glm::dvec3 &obs, const glm::dmat3 &rot,
        const double fov, const double aspect,
        const double faintest);

    bool pickSystem(secondaries_t &cbodies);
    Celestial *pickPlanet(pSystem *system, const glm::dvec3 &obs, const glm::dvec3 &dir, double when);
    Celestial *pick(const glm::dvec3 &obs, const glm::dvec3 &dir, double when);

private:
    StarDatabase stardb;
    Constellations constellations;

    std::vector<pSystem *> systemList;
    SystemsList  systems;

    std::vector<const CelestialStar *> nearStars;
};