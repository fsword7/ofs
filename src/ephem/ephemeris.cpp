// ephemeris.cpp - Orbital Ephemeris Data package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#include "main/core.h"
#include "ephem/ephemeris.h"

// #include "ephem/earth/earth.h"

OrbitEphemeris::OrbitEphemeris(Celestial &cbody)
: cbody(cbody)
{
    
}

// OrbitEphemeris *OrbitEphemeris::create(CelestialBody &cbody, cstr_t &name)
// {

//     return nullptr;
// }
