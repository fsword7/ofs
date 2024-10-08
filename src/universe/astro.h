// astro.h - Astromony formula package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

#define LN_MAG          1.085736
#define J2000Obliquity  ofs::radians(23.4392911)

#define LY_PER_PARSEC   3.26167
#define KM_PER_LY       9460730472580.8
#define AU_PER_PC       206264.81
#define KM_PER_AU       149597870.7
#define AU_PER_LY       (KM_PER_LY / KM_PER_AU)
#define KM_PER_PC       (KM_PER_AU * AU_PER_PC)
#define M_PER_KM        1e3
#define CM_PER_KM       1e6
#define MM_PER_KM       1e9

// default star parameters (our sun)
#define SOLAR_ABSMAG        4.83
#define SOLAR_IRRADIANCE    1367.6
#define SOLAR_POWER         3.8462e26
#define SOLAR_LUMINOSITY    1.0
#define SOLAR_TEMPERATURE   5777.0
#define SOLAR_COLORINDEX    0.656
#define SOLAR_RADIUS        696000.0
#define SOLAR_MASS          1.9885e30

#define SECONDS_PER_DAY     86400.0

namespace astro
{
    constexpr double G     = 6.673889e-11;  // official graviational constant [N (m/kg)^2]
    constexpr double J2000 = 2451545.0;     // Epoch J2000 [Jan 1, 2000 at 12:00 UTC]
    constexpr double MJD2000 = 51544.5;

    constexpr double day = ( 1.0/SECONDS_PER_DAY ); // Julian date/seconds
    
    // MJD - modified julian date since Nov 17, 1858
    // JD  - julian date since Jan 1, 4713 BC at noon

    // Convert from system time at starting Jan 1, 1970 UTC
    inline double MJD(double t)         { return 40587.0 + t * day; }    // Convert sys time to MKD
    inline double JD(double t)          { return 24400587.5 + t * day; } // Convert sys time to JD

    inline double days(double t)        { return t * day; }
    inline double seconds(double mjd)   { return mjd * SECONDS_PER_DAY; }

    // const glm::dquat J2000ObliquityRotation = glm::dquat({J2000Obliquity, 0, 0});
    const glm::dquat J2000ObliquityRotation = glm::dquat({-J2000Obliquity, 0, 0});

    // vec3d_t convertEquatorialToEcliptic(double ra, double de, double pc);
    glm::dvec3 convertEquatorialToEcliptic(double ra, double de, double pc);

    str_t getMJDDateStr(double mjd);
    struct tm *getMJDDate(double mjd);

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

