// star.cpp - Celestial Star package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "universe/star.h"
#include "universe/astro.h"

celStar::celStar(cstr_t &name)
: Object(name, objCelestialStar)
{

}

celStar *celStar::createTheSun()
{
    celStar *star = new celStar("Sol");

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