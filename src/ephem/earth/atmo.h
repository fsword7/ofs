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

    void getAtmParams(const inAtmParam &in, AtmParam &out);

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