// spice.h - NAIF SPICE Interface package
//
// Author:  Tim Stark
// Date:    Oct 12, 2023

#pragma once

class OrbitSPICE : public OrbitEphemeris
{
public:
    OrbitSPICE(CelestialBody &cbody);
    virtual ~OrbitSPICE();

    // static OrbitEphemeris *create(CelestialBody &cbody, cstr_t &name);

protected:
    // void init();
    
    void getEphemeris(double mjd, double *res);

private:
    double beginKernel = 0;
    double endKernel = 0;

    int idBody = 0;
    int idBary = 0;
    int idOrigin = 0;
};