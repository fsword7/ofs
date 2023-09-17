// ephemeris.cpp - Orbital Ephemeris Data package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#include "main/core.h"
#include "ephem/ephemeris.h"

OrbitEphemeris::OrbitEphemeris(CelestialBody &cbody)
: cbody(cbody)
{
    
}