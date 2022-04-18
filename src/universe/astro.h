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

};