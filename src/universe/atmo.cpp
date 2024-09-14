// atmo.cpp - Atmospheric pressure/scattering package
//
// Author:  Tim Stark
// Date:    Jul 9, 2022

#include "main/core.h"
#include "universe/atmo.h"

#include "ephem/earth/atmo.h"

Atmosphere *Atmosphere::create(str_t &name)
{
    if (name == "msise00-earth")
        return new AtmosphereEarthNRLMSISE00();
    return nullptr;
}