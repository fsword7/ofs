// atmo.cpp - Mars atomspheric model 2006 edition
//
// Author:  Tim Stark
// Date:    Sep 15, 2024

#include "main/core.h"
#include "ephem/mars/atmo.h"

str_t AtmosphereMars2006::getsAtmName() const
{
    return "Mars 2006 Edition Model";
}

void AtmosphereMars2006::getAtmConstants(atmconst_t &atmc) const
{
    // Assign atomspheric constants;
    atmc.altLimit = 100;
    atmc.p0 = 0.61e3;
    atmc.rho0 = 0.02;
    atmc.R = 188.92;
    atmc.gamma = 1.2941;
}

void AtmosphereMars2006::getAtmParams(const iatmprm_t &in, atmprm_t &out)
{

}
