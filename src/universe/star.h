// star.h - Celestial Star package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

#include "engine/object.h"

enum SpectralClass
{
    spectralUnknown = 0,
    spectralO  = 1,
    spectralB  = 2,
    spectralA  = 3,
    spectralF  = 4,
    spectralG  = 5,
    spectralK  = 6,
    spectralM  = 7,
    specitalR  = 8,
    spectralS  = 9,
    spectralN  = 10,
    spectralWC = 11,
    spectralWN = 12,
    spectralL  = 13,
    spectralT  = 14,
    spectralC  = 15,

    // Dwarf stars spectral types
    spectralDA = 16,
    spectralDB = 17,
    spectralDC = 18,
    spectralDO = 19,
    spectralDQ = 20,
    spectralDZ = 21,
    spectralD  = 22,
    spectralDX = 23 
};

class celStar : public Object
{
public:
    enum {
        cbKnownRadius   = 1,
        cbKnownRotation = 2,
        cbKnownSurface  = 4,
        cbKnownObject   = (cbKnownRadius|cbKnownRotation|cbKnownSurface)
    };

    celStar(cstr_t &name);
    ~celStar() = default;

    static celStar *createTheSun();

private: 
    // Star catalogue parameters
    uint32_t hip;           // Hippocrais number

    // Star position parameters
    vec3d_t spos;           // Star position [pc]
    double  ra, dec, plx;

    uint32_t knownFlags = 0; // Known flags

    // Spectral class/type
    str_t   specName;
    str_t   specType;

    // Star atomsphere parameters
    double  absMag;         // Absolute magnitude
    double  bMag, vMag;     // Blue/visual magnitude
    double  ci, lum;        // Color index, luminosity
    int     temp;           // Surface temperature
};