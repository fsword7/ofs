// celestial.cpp - Celestial Object package
//
// Author:  Tim Stark
// Date:    Aug 4, 2024

#include "main/core.h"
#include "engine/celestial.h"

Celestial::Celestial(YAML::Node &config, ObjectType type)
: Object(config, type)
{

}


glm::dvec3 Celestial::interpolatePosition(double n) const
{
    if (n == 0.0)
        return s0.pos;
    else if (n == 1.0)
        return s1.pos;
    
    const double eps = 1e-2;
    glm::dvec3 refp0(0, 0, 0);
    glm::dvec3 refp1(0, 0, 0);
    glm::dvec3 refpm(0, 0, 0);

    Celestial *cbody = nullptr; // getReferenceOrbit();
    if (cbody != nullptr)
    {
        refp0 = cbody->s0.pos;
        refp1 = cbody->s1.pos;
        refpm = cbody->interpolatePosition(n);
    }

    glm::dvec3 rp0 = s0.pos - refp0;
    glm::dvec3 rp1 = s1.pos - refp1;
    double rd0 = glm::length(rp0);
    double rd1 = glm::length(rp1);
    double n0 = 0.0;
    double n1 = 1.0;
    double nm = 0.5, d = 0.5;
    double rdm = (rd0 + rd1) * 0.5;
    glm::dvec3 rpm = glm::normalize(rp0 + rp1) * rdm;

    while (fabs(nm - n) > eps && d > eps)
    {
        d *= 0.5;
        if (nm < n) {
            rp0 = rpm, rd0 = rdm;
            n0 = nm, nm += d;
        } else {
            rp1 = rpm, rd1 = rdm;
            n1 = nm, nm -= d;
        }
        rdm = (rd0 + rd1) * 0.5;
        rpm = glm::normalize(rp0 + rp1) * rdm;
    }

    if (fabs(nm - n) > 1e-10)
    {
        double scale = (n - n0) / (n1 - n0);
        rdm = rd0 + (rd1 - rd0)*scale;
        rpm = glm::normalize(rp0 + (rp1 - rp0)*scale) * rdm;
    }

    return rpm + refpm;
}