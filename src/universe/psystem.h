// psystem.h -      Solar (Planetary) System package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#pragma once

class Universe;
class Celestial;
class CelestialStar;
class CelestialBody;
class TimeDate;
class pSystem
{
public:
    pSystem() = default;
    pSystem(str_t &name);
    pSystem(CelestialStar *star);
    ~pSystem() = default;

    void addStar(CelestialStar *cbody);
    void addBody(CelestialBody *cbody);
    void addPlanet(CelestialBody *planet, CelestialBody *cbody);
    void addGravity(Celestial *grav);

    CelestialBody *find(cstr_t &name) const;

    glm::dvec3 addSingleGravityPerturbation(const glm::dvec3 &gpos, const Celestial *body) const;
    glm::dvec3 addSingleGravity(const glm::dvec3 &gpos, const Celestial *body) const;

    glm::dvec3 addGravity(const glm::dvec3 &gpos, const Celestial *exclude = nullptr) const;
    glm::dvec3 addGravityIntermediate(const glm::dvec3 &gpos, double tfrac, const Celestial *exclude = nullptr) const;

    // static bool loadSystems(Universe *universe, cstr_t &sysName);
    // static bool loadPlanet(const cstr_t &cbName, pSystem *psys, fs::path cbFolder);

    static bool loadStar(const cstr_t &cbName, Universe *universe, pSystem *psys, cstr_t &cbFolder);
    static bool loadPlanet(const cstr_t &cbName, pSystem *psys, cstr_t &cbFolder);
    static bool loadSystems(Universe *universe, cstr_t &sysName);

    void update(const TimeDate &td);
    void finalizeUpdate();

private:
    str_t sysName;

    CelestialStar *primaryStar = nullptr;

    std::vector<CelestialStar *> stars;
    std::vector<CelestialBody *> bodies;
    std::vector<CelestialBody *> planets;
    std::vector<Celestial *> gravities;
};