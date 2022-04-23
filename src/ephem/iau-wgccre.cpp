// iau-wgccre.cpp - IAU WGCCRE Rotational elements package
//
// WGCCRE provides some papers fdor IAU rotational elements below.
//
// Using with body equatorial frame.
//
// https://astrogeology.usgs.gov/groups/IAU-WGCCRE
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "ephem/rotation.h"
#include "universe/astro.h"

// ******** IAU Rotational Model ********

quatd_t IAURotation::computeSpin(double tjd) const
{
    tjd -= astro::J2000;

    double spin = ((reversal == true) ? 1 : -1) *
        glm::radians(180.0 + computeMeridian(tjd));
    return yRotate(spin);
}

quatd_t IAURotation::computeEquatorRotation(double tjd) const
{
    tjd -= astro::J2000;

    vec2d_t pole = computePole(tjd);

    double node = pole.x + 90.0;        // Right ascension [RA]
    double inclination = 90.0 - pole.y; // Declination [DE]

    if (reversal != true)
        return xRotate(pi) * xRotate(glm::radians(-inclination)) * yRotate(glm::radians(-node));
    else
        return xRotate(glm::radians(-inclination)) * yRotate(glm::radians(-node));
}

// ******** IAU Precission Rotational Model ********

vec2d_t IAUPrecissionRotation::computePole(double tjd) const
{
    double t = tjd / 36525.0;
    vec2d_t pole;

    pole.x = poleRA + (poleRARate * tjd);
    pole.y = poleDec + (poleDecRate * tjd);

    return pole;
}

double IAUPrecissionRotation::computeMeridian(double tjd) const
{
    return meridianAtEpoch + (rotationRate * tjd);
}

// ******** Rotational Model Creation ********

Rotation *Rotation::create(cstr_t &name)
{
    if (name == "p03lp-earth")
    {
        fmt::printf("Using P03LP rotation model...\n");
        return new EarthRotation();
    }

    if (name == "iau-sun")
        return new IAUPrecissionRotation(
            286.33,   0.0,
             63.87,   0.0,
             84.176, 14.1844000
        );

    if (name == "iau-mercury")
        return new IAUPrecissionRotation(
            281.0097, -0.0328,
             61.4143, -0.0049,
            329.5469,  6.1385025
        );
    
    if (name == "iau-venus")
        return new IAUPrecissionRotation(
            272.16,  0.0,
             67.16,  0.0,
            160.20, -1.4813688
        );

    if (name == "iau-earth")
        return new IAUPrecissionRotation(
              0.0,    -0.641,
             90.0,    -0.557,
            190.147, 360.9856235
        );

    if (name == "iau-mars")
        return new IAUPrecissionRotation(
            317.68143, -0.1061,
             52.537,   -0.0609,
            176.630,  350.8919822
        );

    if (name == "iau-saturn")
        return new IAUPrecissionRotation(
            40.589,  -0.36,
            83.537,  -0.004,
            38.90,  810.7939024
        );

    if (name == "iau-uranus")
        return new IAUPrecissionRotation(
             257.311,   0.0,
             -15.175,   0.0,
             203.81, -501.1600928
        );

    // if (name == "iau-neptune")
    //     return new IAUNeptuneRotationalModel();

    // if (name == "iau-lunar")
    //     return new IAULunarRotationalModel();

    return nullptr;
}