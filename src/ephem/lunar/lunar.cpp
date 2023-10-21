// lunar.cpp - ELP82 epheremeis data package
//
// Author:  Tim Stark
// Date:    Oct 20, 2023

#include "main/core.h"
#include "api/celbody.h"
#include "ephem/ephemeris.h"
#include "ephem/lunar/elp82.h"
#include "ephem/lunar/lunar.h"

OrbitELP82Lunar::OrbitELP82Lunar(CelestialBody &cbody)
: OrbitELP82(cbody)
{
}

uint16_t OrbitELP82Lunar::getOrbitData(double mjd, uint16_t req, double *res)
{
    getEphemeris(mjd, res);
    if (req & (EPHEM_BARYPOS|EPHEM_BARYVEL))
        for (int idx = 6; idx < 12; idx++)
            res[idx] = res[idx-6];
    return req | EPHEM_TRUEBARY | EPHEM_TRUEPOS | EPHEM_TRUEVEL;
}
