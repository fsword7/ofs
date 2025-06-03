// atmo.h - Earth atomspheric model NRLMSISE-00 package
//
// Author:  Tim Stark
// Date:    Jul 9, 2022

#pragma once

// #include "ephem/sol/earth/nrlmsise-00.h"
#include "ephem/sol/earth/nrlmsise00.hpp"
#include "ephem/sol/earth/nrlmsise00_p.hpp"

#include "universe/atmo.h"

class AtmosphereEarthNRLMSISE00 : public Atmosphere
{
public:
    AtmosphereEarthNRLMSISE00();

    str_t getsAtmName() const override;
    void getAtmConstants(atmconst_t &atmc) const override;
    void getAtmParams(const iatmprm_t &in, atmprm_t &out) override;

private:
    atmos::CNrlmsise00 atm;

    const std::array<int, 24> swFlags = 
        {
            0, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1
        };
};