// sol.cpp - Sun/Sol VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Sep 18, 2023

#include "main/core.h"
#include "api/celbody.h"
#include "ephem/vsop87/vsop87.h"
#include "ephem/vsop87/sol.h"

OrbitVSOP87Sol::OrbitVSOP87Sol(Celestial &cbody, vsop87p_t &series)
: OrbitVSOP87(cbody, series)
{
    a0 = 1.0; // semi-major axis [AU]
}

uint16_t OrbitVSOP87Sol::getOrbitData(double mjd, uint16_t req, double *res)
{
    // Calculate position/velocity for star wobble
    if (req & (EPHEM_TRUEPOS|EPHEM_TRUEVEL))
        getEphemeris(mjd, res);

    // Barycentric origin is always zero for planetary system
    if (req & (EPHEM_BARYPOS|EPHEM_BARYVEL))
        for (int idx = 6; idx < 12; idx++)
            res[idx] = 0.0;

    return (req & EPHEM_POSVEL) | fmtFlags;
}