// atmo.h - Earth atomspheric model NRLMSISE-00 package
//
// Author:  Tim Stark
// Date:    Jul 9, 2022

#pragma once

#include "universe/atmo.h"

class AtomsphereEarthNRLMSISE00 : public Atomsphere
{
public:
    AtomsphereEarthNRLMSISE00();

    void getAtmConstants(atmconst_t &atmc);
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