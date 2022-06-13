// astro.cpp - OFS astronomy formula package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/astro.h"

namespace astro
{
    static const quatd_t J2000ObliquityRotation =
        Eigen::Quaternion<double>(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX()));
 
    vec3d_t convertEquatorialToEcliptic(double ra, double de, double pc)
    {
        double theta, phi;
        double x, y, z;
        vec3d_t opos;

        theta = ofs::radians(ra) + pi;
        phi   = ofs::radians(de) - pi/2.0;

        opos  = vec3d_t(sin(phi)*cos(theta), cos(phi), sin(phi)*-sin(theta)) * pc;

        return J2000ObliquityRotation * opos;

        // return opos * quatd_t(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX())) * pc;
    }
}