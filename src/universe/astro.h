// astro.h - Astromony formula package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#define LN_MAG          1.085736
#define J2000Obliquity  glm::radians(23.4392911)

#define LY_PER_PARSEC   3.26167
#define KM_PER_LY       9460730472580.8
#define AU_PER_PC       206264.81
#define KM_PER_AU       149597870.7
#define AU_PER_LY       (KM_PER_LY / KM_PER_AU)
#define KM_PER_PC       (KM_PER_AU * AU_PER_PC)

// default star parameters (our sun)
#define SOLAR_ABSMAG        4.83
#define SOLAR_IRRADIANCE    1367.6
#define SOLAR_POWER         3.8462e26
#define SOLAR_LUMINOSITY    1.0
#define SOLAR_TEMPERATURE   5777.0
#define SOLAR_COLORINDEX    0.656
#define SOLAR_RADIUS        696000.0
#define SOLAR_MASS          1.9885e30

namespace astro
{
    constexpr double G     = 6.673889e-11;  // official graviational constant [N (m/kg)^2]
    constexpr double J2000 = 2451545.0;     // Epoch J2000 [Jan 1, 2000 at 12:00 UTC]

    vec3d_t convertEquatorialToEcliptic(double ra, double de, double pc);

    template <typename T> T convertKilometerToAU(T km)
    {
        return km / (T)KM_PER_AU;
    }

    template <typename T> T convertKilometerToParsec(T km)
    {
        return km / (T)KM_PER_PC;
    }

    template <typename T> T convertAUToKilometer(T au)
    {
        return au * (T)KM_PER_AU;
    }
    
    template <typename T> T convertParsecToKilometer(T pc)
    {
        return pc * (T)KM_PER_PC;
    }


    template <typename T> T convertAbsToAppMag(T absMag, T pc)
    {
        return absMag - 5 + 5 * log10(pc);
    }

    template <typename T> T convertAppToAbsMag(T appMag, T pc)
    {
        return appMag + 5 - 5 * log10(pc);
    }

    template <typename T> T convertAbsMagToLum(T absMag)
    {
        return exp((SOLAR_ABSMAG - absMag) / LN_MAG);
    }

    template <typename T> T convertLumToAbsMag(T lum)
    {
        return (SOLAR_ABSMAG - log(lum) * LN_MAG);
    }

    template <typename T> T convertAppMagToLum(T appMag, T pc)
    {
        return convertAbsMagToLum(convertAppToAbsMag(appMag, pc));
    }

    template <typename T> T convertLumToAppMag(T lum, T pc)
    {
        return convertAbsToAppMag(convertLumToAbsMag(lum), pc);
    }

};

