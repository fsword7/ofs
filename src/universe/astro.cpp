// astro.cpp - OFS astronomy formula package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/astro.h"

namespace astro
{
    vec3d_t convertEquatorialToEcliptic(double ra, double de, double pc)
    {
        double theta, phi;
        double x, y, z;

        vec3d_t rot(J2000Obliquity, 0, 0);
        vec3d_t opos;

        theta = glm::radians(ra) + pi;
        phi   = glm::radians(de) - pi/2.0;

        opos  = vec3d_t(sin(phi)*cos(theta), cos(phi), sin(phi)*-sin(theta));

        return opos * glm::dquat(rot) * pc;
    }
}