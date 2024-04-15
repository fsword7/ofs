// star.h - Celestial Star package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

#include "universe/celbody.h"

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

class System;
class pSystem;

class CelestialStar : public CelestialBody
{
public:
    CelestialStar(cstr_t &name);
    ~CelestialStar() = default;

    // Solar/Planetary system
    inline void setpSystem(pSystem *psys)       { systemx = psys; }
    inline bool haspSystem() const              { return systemx != nullptr; }
    inline pSystem *getpSystem() const          { return systemx; }

    inline glm::dvec3 getStarPosition() const   { return spos; }
    inline double  getAbsMag() const            { return absMag; }
    inline int getTemperature() const           { return temp; }
    inline double getLuminosity() const         { return lum; }
    
    inline void setHIPnumber(int val)           { hip = val; }
    inline uint32_t getHIPnumber() const        { return hip; }

    inline bool hasSolarSystem() const          { return system != nullptr; }
    inline System *getSolarSystem() const       { return system; }
    inline void setSolarSystem(System *sys)     { system = sys; }

    static CelestialStar *createTheSun();
    static CelestialStar *create(double ra, double de, double pc,
        cchar_t *spType, double appMag, double ci, double lum);

    void configure(YAML::Node &config);

private: 
    // Star catalogue parameters
    uint32_t hip;           // Hippocrais number

    // Star position parameters
    glm::dvec3 spos;        // Star pisition [pc]
    double  ra, dec, plx;

    // Spectral class/type
    str_t   specName;
    str_t   specType;

    // Star atomsphere parameters
    double  absMag;         // Absolute magnitude
    double  bMag, vMag;     // Blue/visual magnitude
    double  ci, lum;        // Color index, luminosity
    int     temp;           // Surface temperature

    pSystem *systemx = nullptr;
    System *system = nullptr;
};