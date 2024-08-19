// ephemeris.h - Orbital Ephemeris Data package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#pragma once

class Celestial;

class OrbitEphemeris
{
public:
    OrbitEphemeris(Celestial &cbody);
    ~OrbitEphemeris() = default;

    // static OrbitEphemeris *create(CelestialBody &cbody, cstr_t &name);

    virtual uint16_t getOrbitData(double mjd, uint16_t req, double *ret) = 0;

protected:
    Celestial &cbody;
};