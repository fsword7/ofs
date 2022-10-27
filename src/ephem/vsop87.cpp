// vsop87.cpp - VSOP87 orbit epheremis package
//
// Author:  Tim Stark
// Date:    Mar 7, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/vsop87.h"
#include "ephem/elp-mpp02.h"

#define VSOP_SERIES(series) vsop87s_t(series, ARRAY_SIZE(series))
#define VSOP_PARAM(series)  (series), ARRAY_SIZE(series)

#include "ephem/vsop87mer.cpp" // Mercury
#include "ephem/vsop87ven.cpp" // Venus
#include "ephem/vsop87ear.cpp" // Earth
#include "ephem/vsop87mar.cpp" // Mars
#include "ephem/vsop87jup.cpp" // Jupiter
#include "ephem/vsop87sat.cpp" // Saturn
#include "ephem/vsop87ura.cpp" // Uranus
#include "ephem/vsop87nep.cpp" // Neptune

VSOP87Orbit::VSOP87Orbit(vsop87s_t *sL, int nL,
                         vsop87s_t *sB, int nB,
                         vsop87s_t *sR, int nR,
                         double period)
: sL(sL), nL(nL),
  sB(sB), nB(nB),
  sR(sR), nR(nR),
  period(period),
  boundingRadius(0.0)
{ }

double VSOP87Orbit::sum(const vsop87s_t &series, double t) const
{
    if (series.nTerms < 1)
        return 0.0;
    
    double x = 0.0;
    vsop87_t *terms = &series.terms[0];

    for (int idx = 0; idx < series.nTerms; idx++, terms++)
        x += terms->a * cos(terms->b + terms->c * t);

    return x;
}

glm::dvec3 VSOP87Orbit::calculatePosition(double jd) const
{
    // Helicentric coordinate system
    double l = 0.0; // longitude
    double b = 0.0; // latiude
    double r = 0.0; // radius

    // Julian time since J2000.0
    double t = (jd - 2451545.0) / 365250.0;
    double T;
    int idx;

    // Evaluate series for L (longitude)
    for (idx = 0, T = 1.0; idx < nL; idx++, T *= t)
        l += sum(sL[idx], t) * T;

    // Evaluate series for B (latitude)
    for (idx = 0, T = 1.0; idx < nB; idx++, T *= t)
        b += sum(sB[idx], t) * T;

    // Evaluate series for R (radius)
    for (idx = 0, T = 1.0; idx < nR; idx++, T *= t)
        r += sum(sR[idx], t) * T;

    // Convert AU to kilometer
    r *= KM_PER_AU;
    l += pi;
    b -= pi / 2.0;

    // Astrocentric coordinate system
    return glm::dvec3( sin(b)*cos(l), cos(b), sin(b)*-sin(l) ) * r;
}

glm::dvec3 VSOP87Orbit::calculateVelocity(double jd) const
{
    return { 0, 0, 0 };
}

Orbit *VSOP87Orbit::create(cstr_t &name)
{
    if (name == "vsop87-mercury")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(mercury_L),
                                       VSOP_PARAM(mercury_B),
                                       VSOP_PARAM(mercury_R),
                                       365.25 * 0.2408);
        return orbit;
    }

    if (name == "vsop87-venus")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(venus_L),
                                       VSOP_PARAM(venus_B),
                                       VSOP_PARAM(venus_R),
                                       365.25 * 0.6152);
        return orbit;
    }

    if (name == "vsop87-earth")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(earth_L),
                                       VSOP_PARAM(earth_B),
                                       VSOP_PARAM(earth_R),
                                       365.25);
        return orbit;
    }

    if (name == "vsop87-mars")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(mars_L),
                                       VSOP_PARAM(mars_B),
                                       VSOP_PARAM(mars_R),
                                       365.25 * 1.8809);
        return orbit;
    }

    if (name == "vsop87-jupiter")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(jupiter_L),
                                       VSOP_PARAM(jupiter_B),
                                       VSOP_PARAM(jupiter_R),
                                       365.25 * 11.86);
        return orbit;
    }

    if (name == "vsop87-saturn")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(saturn_L),
                                       VSOP_PARAM(saturn_B),
                                       VSOP_PARAM(saturn_R),
                                       365.25 * 29.4577);
        return orbit;
    }

    if (name == "vsop87-uranus")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(uranus_L),
                                       VSOP_PARAM(uranus_B),
                                       VSOP_PARAM(uranus_R),
                                       365.25 * 84.0139);
        return orbit;
    }

    if (name == "vsop87-neptune")
    {
        Orbit *orbit = new VSOP87Orbit(VSOP_PARAM(neptune_L),
                                       VSOP_PARAM(neptune_B),
                                       VSOP_PARAM(neptune_R),
                                       365.25 * 164.793);
        return orbit;
    }

    if (name == "elp-mpp02-llr-lunar")
    {
        Orbit *orbit = new ELP2000Orbit(ELP2000Orbit::elpUseLLR);
        return orbit;
    }

    if (name == "elp-mmp02-de406-lunar")
    {
        Orbit *orbit = new ELP2000Orbit(ELP2000Orbit::elpUseDE406);
        return orbit;
    }

    return nullptr;
}