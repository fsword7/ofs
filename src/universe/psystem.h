// psystem.h -      Solar (Planetary) System package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#pragma once

class Universe;
class Celestial;
class CelestialStar;
class CelestialBody;
class Vehicle;
class TimeDate;
class pSystem
{
public:
    pSystem() = default;
    pSystem(str_t &name);
    pSystem(CelestialStar *star);
    ~pSystem() = default;

    void addStar(CelestialStar *cbody);
    void addBody(Celestial *cbody);
    void addPlanet(CelestialBody *planet, CelestialBody *cbody);
    void addVehicle(Vehicle *vehicle);
    void addCelestial(Celestial *cel);

    Celestial *find(cstr_t &name) const;

    bool removeVehicle(Vehicle *);
    inline Vehicle *getVehicle(int idx) const   { return idx < vehicles.size() ? vehicles[idx] : nullptr; };
    Vehicle *getVehicle(cstr_t &name, bool incase = true) const;

    glm::dvec3 addSingleGravityPerturbation(const glm::dvec3 &gpos, const Celestial *body) const;
    glm::dvec3 addSingleGravity(const glm::dvec3 &gpos, const Celestial *body) const;

    glm::dvec3 addGravity(const glm::dvec3 &gpos, const Celestial *exclude = nullptr) const;
    glm::dvec3 addGravityIntermediate(const glm::dvec3 &gpos, double tfrac, const Celestial *exclude = nullptr) const;

    // static bool loadSystems(Universe *universe, cstr_t &sysName);
    // static bool loadPlanet(const cstr_t &cbName, pSystem *psys, fs::path cbFolder);

    static bool loadStar(const cstr_t &cbName, Universe *universe, pSystem *psys, cstr_t &cbFolder);
    static bool loadPlanet(const cstr_t &cbName, pSystem *psys, cstr_t &cbFolder);
    static bool loadSystems(Universe *universe, cstr_t &sysName);

    void update(bool force);
    void finalizeUpdate();

private:
    str_t sysName;

    CelestialStar *primaryStar = nullptr;

    std::vector<CelestialStar *> stars;
    std::vector<Celestial *> bodies;
    std::vector<Celestial *> planets;
    std::vector<Vehicle *> vehicles;
    std::vector<Celestial *> celestials;

};