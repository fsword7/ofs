// astro.cpp - OFS astronomy formula package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/astro.h"

namespace astro
{ 
    // vec3d_t convertEquatorialToEcliptic(double ra, double de, double pc)
    // {
    //     double theta, phi;
    //     double x, y, z;
    //     vec3d_t opos;

    //     theta = ofs::radians(ra) + pi;
    //     phi   = ofs::radians(de) - pi/2.0;

    //     opos  = vec3d_t(sin(phi)*cos(theta), cos(phi), sin(phi)*-sin(theta)) * pc;

    //     return J2000ObliquityRotation * opos;
    // }

    glm::dvec3 convertEquatorialToEcliptic(double ra, double de, double pc)
    {
        double theta, phi;
        double x, y, z;
        glm::dvec3 opos;

        theta = ofs::radians(ra) + pi;
        phi   = ofs::radians(de) - pi/2.0;

        opos  = glm::dvec3(sin(phi)*cos(theta), cos(phi), sin(phi)*-sin(theta)) * pc;

        return J2000ObliquityRotation2 * opos;
    }

}