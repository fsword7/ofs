// earth.cpp - Earth VSOP87 ephemeris package
//
// Author:  Tim Stark
// Date:    Nov 5, 2022

#include "main/core.h"
#include "api/celbody.h"
#include "ephem/vsop87/vsop87.h"
#include "ephem/vsop87/neptune.h"

OrbitVSOP87Neptune::OrbitVSOP87Neptune(Celestial &cbody, vsop87p_t &series)
: OrbitVSOP87(cbody, series)
{
    a0 = 30.1; // semi-major axis [AU]
}

uint16_t OrbitVSOP87Neptune::getOrbitData(double mjd, uint16_t req, double *res)
{
    getEphemeris(mjd, res);
    return fmtFlags | EPHEM_TRUEPOS | EPHEM_TRUEVEL;
}