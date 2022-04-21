// system.h - solar system package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#pragma once

class celStar;

#include "universe/body.h"

class System
{
public:
    System(celStar *star);
    ~System() = default;

    inline PlanetarySystem *getPlanetarySystem()    { return &objects; }

    static celBody *createBody(cstr_t &name, PlanetarySystem *system,
        celType type, cstr_t &orbitFrameName, cstr_t &bodyFrameName);

private:
    std::vector<celStar *> stars; // Multi-star systems
    PlanetarySystem objects;
};

typedef std::map<uint32_t, System *> SystemsList;