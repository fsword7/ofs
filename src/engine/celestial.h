// celestial.h - Celestial Object package
//
// Author:  Tim Stark
// Date:    Aug 4, 2024

#pragma once

#include "engine/object.h"

class pSystem;

class OFSAPI Celestial : public Object
{
public:
    Celestial(cstr_t &name, ObjectType type)
    : Object(name, type)
    { }

    Celestial(YAML::Node &config, ObjectType type);

    inline void setPlanetarySystem(pSystem *sys)       { system = sys; }
    inline pSystem *getPlanetarySystem() const          { return system; }

    inline void flushPosition()                 { brpos = irpos, irpos = {}; }
    inline void flushVelocity()                 { brvel = irvel, irvel = {}; }

    glm::dvec3 interpolatePosition(double frac) const;

protected:
    pSystem *system = nullptr;
    Celestial *cbody = nullptr;    // orbit reference body

    glm::dvec3 brpos, irpos;    // relative position (base and incremental)
    glm::dvec3 brvel, irvel;    // relative velocity (base and incremantel)
};
