// ephemeris.h - Orbital Ephemeris Data package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#pragma once

class CelestialBody;

class OrbitEphemeris
{
public:
    OrbitEphemeris(CelestialBody &cbody);
    ~OrbitEphemeris() = default;
    
    virtual uint16_t getOrbitData(double mjd, uint16_t req, double *ret) = 0;

protected:
    CelestialBody &cbody;
};