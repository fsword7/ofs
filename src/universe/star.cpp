// star.cpp - Celestial Star package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "universe/body.h"
#include "universe/star.h"
#include "universe/astro.h"

CelestialStar::CelestialStar(cstr_t &name)
: CelestialBody(name, cbStar)
{

}

CelestialStar *CelestialStar::createTheSun()
{
    CelestialStar *star = new CelestialStar("Sol");

    star->ra            = 0.0;
    star->dec           = 0.0;
    star->plx           = 0.0;
    star->absMag        = SOLAR_ABSMAG;
    star->bMag          = 0.0;
    star->vMag          = 0.0;
    star->ci            = SOLAR_COLORINDEX;
    star->lum           = SOLAR_LUMINOSITY;
    star->temp          = SOLAR_TEMPERATURE;

    star->knownFlags    = cbKnownObject;
    star->radius        = SOLAR_RADIUS;
    star->mass          = SOLAR_MASS;

    star->hip           = 0;
    star->spos          = { 0, 0, 0 };
    star->objPosition   = { 0, 0, 0 };
    // star->orbitFrame    = nullptr;

    return star;
}

CelestialStar *CelestialStar::create(double ra, double de, double pc,
    cchar_t *spType, double appMag, double ci, double lum)
{
    CelestialStar *star = new CelestialStar("(unknown)");
    int temp;

    star->spos = astro::convertEquatorialToEcliptic(ra, de, pc);
    star->objPosition = star->spos * KM_PER_PC;

    temp = (int)(4600 * (1.0 / ((ci * 0.92) + 1.7) + 1.0 / ((ci * 0.92) + 0.62)));

    star->ra   = ra;
    star->dec  = de;
    star->absMag = astro::convertAppToAbsMag(appMag, pc);
    star->bMag = appMag + ci;
    star->vMag = appMag;
    star->ci   = ci;
    star->lum  = lum;
    star->temp = temp;

    return star;
}
