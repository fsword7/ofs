// atmo.cpp - Mars atomspheric model 2006 edition
//
// Author:  Tim Stark
// Date:    Sep 15, 2024

#include "main/core.h"
#include "ephem/venus/atmo.h"

str_t AtmosphereVenus2006::getsAtmName() const
{
    return "Venus 2006 Edition Model";
}

void AtmosphereVenus2006::getAtmConstants(atmconst_t &atmc) const
{
    // Assign atomspheric constants;
    atmc.altLimit = 200;
    atmc.p0 = 9200e3;
    atmc.rho0 = 65;
    atmc.R = 188.92;
    atmc.gamma = 1.2857;
}

void AtmosphereVenus2006::getAtmParams(const iatmprm_t &in, atmprm_t &out)
{

}
