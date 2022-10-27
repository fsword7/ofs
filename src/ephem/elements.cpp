// elements.cpp - orbital elements package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "ephem/elements.h"

const double G       = 6.67259e-11; // gravitational constant [m^3 kg^-1 s^-2]
const double MJD2000 = 51544.5;     // MJD date for epoch J2000
// const double pi      = 3.14159265358979323846;

OrbitalElements::OrbitalElements(double a, double e, double i,
    double theta, double omegab, double L, double mjd)
: a(a), e(e), i(i), theta(theta), omegab(omegab), L(L),
  mjdEpoch(mjd)
{ }

void OrbitalElements::setMasses(double _m, double _M)
{
    m  = _m;
    M  = _M;
    mu = G * (m + M);
}

// determine orbital data (free fall) by using masses
void OrbitalElements::setup(double m, double M, double mjd)
{
    setMasses(m, M);

    le = a * e;     // linear eccentricity
    ad = a + le;    // apoapsis distance
    pd = a - le;    // periapsis distance
    omega = omegab - theta;

    if (e < 1.0)
    {
        // circular/elliptical orbit
        b = a * sqrt(1.0 - e*e);
        n = sqrt(mu / (a*a*a));
        tmp = sqrt((1.0 + e) / (1.0 - e));
    }
    else
    {
        // parabolic/hyperbolic orbit
        b = a * sqrt(e*e - 1.0);
        n = sqrt(-mu / (a*a*a));
    }

    // set time parameters
    double mjdStart = 0.0;
    T = (pi * 2.0) / n;                  // orbital period
    L += (mjd - mjdEpoch) * 86400.0 * n; // mean longitude
    mjdEpoch = mjd;
    tEpoch = (mjdEpoch - mjdStart) * 86400.0;
    tau = tEpoch - (L - omegab) / n;
}

// determine new orbital path
void OrbitalElements::calculate(const glm::dvec3 &pos, const glm::dvec3 &vel, double t)
{
    // Set radius/velocity vectors
    R = pos;
    V = vel;
    r = glm::length(R);
    v = glm::length(V);

    // a = r * mu / (2.0 * mu - r * V.squaredNorm());

}

// updating position and velocity periodically
void OrbitalElements::update(glm::dvec3 &pos, glm::dvec3 &vel)
{

}