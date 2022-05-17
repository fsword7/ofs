// system.h - solar system package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#pragma once

class celStar;
class Universe;
class Parser;
class Group;

#include "universe/body.h"

class System
{
public:
    System(celStar *star);
    ~System() = default;

    inline PlanetarySystem *getPlanetarySystem()    { return &objects; }

    static celBody *createBody(cstr_t &name, PlanetarySystem *system,
        celType type, cstr_t &orbitFrameName, cstr_t &bodyFrameName);

    // Loading SSO system file
    static bool logError(const Parser &parser, cstr_t &message);
    static void setSurface(celSurface &surface, Group *objData);
    static celBody *createBody2(cstr_t &name, celType type, PlanetarySystem *pSystem,
        Group *objData);
    static bool loadSolarSystemObjects(std::istream &in, Universe &universe, const fs::path &path);

private:
    std::vector<celStar *> stars; // Multi-star systems
    PlanetarySystem objects;
};

typedef std::map<uint32_t, System *> SystemsList;