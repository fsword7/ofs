// spice.cpp - NAIF SPICE Interface package
//
// Author:  Tim Stark
// Date:    Oct 12, 2023

#include "main/core.h"
#include "api/celbody.h"
#include "ephem/ephemeris.h"
#include "ephem/spice.h"

OrbitSPICE::OrbitSPICE(Celestial &cbody)
: OrbitEphemeris(cbody)
{
}

OrbitSPICE::~OrbitSPICE()
{
}

void OrbitSPICE::getEphemeris(double mjd, double *res)
{
    double state[6] = {};
    double lt;
    int resp = 0;
    int req = 0;

    double t = (mjd - 51544.5) * 86400.0;

    // Clear all contents first
    for (int idx = 0; idx < 12; idx++)
        res[idx] = 0;

    if ((t >= beginKernel) && (t <= endKernel))
    {
        // SPICE routine processing
        lt = 0.0;
        // spkgeo_t(idBody, t, "ECLIPJ2000", idOrigin, state, &lt);
        if (req & (EPHEM_TRUEPOS|EPHEM_TRUEVEL))
        {
            res[6]  = res[0] = state[0] * 1000.0;
            res[8]  = res[2] = state[1] * 1000.0;
            res[7]  = res[1] = state[2] * 1000.0;
            res[9]  = res[3] = state[3] * 1000.0;
            res[11] = res[5] = state[4] * 1000.0;
            res[10] = res[4] = state[5] * 1000.0;
            resp |= EPHEM_TRUEPOS|EPHEM_TRUEVEL;
        }

        if ((idBody != 10) && (req & (EPHEM_BARYPOS|EPHEM_BARYVEL)))
        {
            if (idBary != idBody)
            {
                lt = 0.0;
                // spkgeo_c(idBary, t, "ECLIPJ2000", idOrigin, state, &lt);
                res[6]  = state[0] * 1000.0;
                res[8]  = state[1] * 1000.0;
                res[7]  = state[2] * 1000.0;
                res[9]  = state[3] * 1000.0;
                res[11] = state[4] * 1000.0;
                res[10] = state[5] * 1000.0;
            }
            else
                resp |= EPHEM_TRUEBARY;
            resp |= EPHEM_BARYPOS|EPHEM_BARYVEL;
        }
    }

    // return resp;
}
