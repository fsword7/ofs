// atmo.h - Venus atomspheric model 2006 edition
//
// Author:  Tim Stark
// Date:    Sep 15, 2024

#pragma once

#include "universe/atmo.h"

class AtmosphereVenus2006 : public Atmosphere
{
public:
    AtmosphereVenus2006() = default;

    str_t getsAtmName() const override;
    void getAtmConstants(atmconst_t &atmc) const override;
    void getAtmParams(const iatmprm_t &in, atmprm_t &out) override;
};