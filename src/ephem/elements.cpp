// elements.cpp - orbital elements package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "ephem/elements.h"

const double G       = 6.67259e-11; // gravitational constant [m^3 kg^-1 s^-2]
const double MJD2000 = 51544.5;     // MJD date for epoch J2000
// const double pi      = 3.14159265358979323846;

static constexpr const double E_CIRCLE_LIMIT = 1e-8;
static constexpr const double I_NOINC_LIMIT  = 1e-8;

OrbitElements::OrbitElements(double a, double e, double i,
    double theta, double omegab, double L, double mjd)
: a(a), e(e), i(i), theta(theta), omegab(omegab), L(L),
  mjdEpoch(mjd)
{ }

void OrbitElements::setMasses(double _m, double _M)
{
    m  = _m;
    M  = _M;
    mu = G * (m + M);
}

double OrbitElements::getEccentricAnomaly(double ma) const
{

    constexpr const double tol = 1e-16;
    constexpr const int maxiter = 16;

    double res;
    double E;

    if (e < 1.0)
    {
        // closed orbit
        E = (fabs(ma-ma0) < 1e-2 ? ma0 : ma);
        res = ma - E + e * sin(E);
        for (int i = 0; fabs(res) > tol && i < maxiter; i++)
        {
            E += std::max(-1.0, std::min(1.0, res / (1.0 - e * cos(E))));
            res = ma - E + e * sin(E);
        }
    } 
    else
    {
        // open orbit
        E = ea0, res = 1.0;
        res = ma - E + e * sin(E);
        for (int i = 0; fabs(res) > tol && i < maxiter; i++)
        {
            E += std::max(-1.0, std::min(1.0, res / (e * cosh(E) - 1.0)));
            res = ma - e * sinh(E) + E;
        }
    }

    ma0 = ma;
    ea0 = E;
    return E;
}

double OrbitElements::getEccentricAnomalyTA(double ta) const
{
    if (e < 1.0)
        return 2.0 * atan(tan(0.5 * ta) / tmp);
    else
    {
        double costa = cos(ta);
        double ea = acosh((e*costa)/(e*costa+1.0));
        return (ta >= 0.0) ? ea : -ea;
    }
}

double OrbitElements::getTrueAnomalyE(double ea) const
{
    if (e < 1.0)
        return 2.0 * atan(tmp * tan(0.5 * ea));
    else
    {
        double coshea = cosh(ea);
        double tra = acos((e - coshea)/(e*coshea - 1.0));
        return (ea >= 0.0) ? tra : -tra;
    }
}

double OrbitElements::getMeanAnomalyE(double ea) const
{
    if (e < 1.0)
        return ea - e*sin(ea);
    else
        return e*sinh(ea) - ea;
}



// determine orbital data (free fall) by using masses
void OrbitElements::setup(double m, double M, double mjd)
{
    setMasses(m, M);

    le = a * e;     // linear eccentricity
    ad = a + le;    // apoapsis distance
    pd = a - le;    // periapsis distance
    omega = omegab - theta;

    p = std::max(0.0, a * (1.0 - a*a));
    muh = sqrt(mu/p);

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

    // Calculate sin/cos of theta, inclination and omega
    sint = sin(theta), cost = cos(theta);
    sini = sin(i),     cosi = cos(i);
    sino = sin(omega), coso = cos(omega);

    // set time parameters
    double mjdStart = 0.0;
    T = (pi * 2.0) / n;                  // orbital period
    L += (mjd - mjdEpoch) * 86400.0 * n; // mean longitude
    mjdEpoch = mjd;
    tEpoch = (mjdEpoch - mjdStart) * 86400.0;
    tau = tEpoch - (L - omegab) / n;
}

// determine new orbital path
void OrbitElements::calculate(const glm::dvec3 &pos, const glm::dvec3 &vel, double t)
{
    // Set radius/velocity vectors
    R = pos;
    V = vel;
    r = glm::length(R);
    v = glm::length(V);

    // a = r * mu / (2.0 * mu - r * V.squaredNorm());

}

// updating position and velocity periodically
void OrbitElements::update(double mjd)
{
    double sinto, costo;
    double vx, vz;
    double thetav;

    if (e < E_CIRCLE_LIMIT)
        ma = ea = tra = n * fmod(mjd - tau, T);
    else
    {
        ma = getMeanAnomaly(mjd);
        if (e < 1.0)
            ma = ofs::posangle(ma);
        ea = getEccentricAnomaly(ma);
        tra = getTrueAnomalyE(ea);
    }

    r = p / (1.0 + e * cos(tra));

    vx = -muh * sin(tra);
    vz = muh * (e + cos(tra));
    thetav = atan2(vz, vx);
    v = sqrt(vx*vx + vz*vz);

    sinto = sin(tra + omega);
    costo = cos(tra + omega);
    R.x = (cost*costo - sint*sinto*cosi) * r;
    R.z = (sint*costo + cost*sinto*cosi) * r;
    R.y = (sinto * sini) * r;

    sinto = sin(thetav + omega);
    costo = cos(thetav + omega);
    V.x = (cost*costo - sint*sinto*cosi) * v;
    V.z = (sint*costo + cost*sinto*cosi) * v;
    V.y = (sinto * sini) * v;

    // pos = R;
    // vel = V;
}


void OrbitElements::getPolarPosition(double t, double &r, double &ta)
{
    if (e < E_CIRCLE_LIMIT)
        ta = n * fmod(t-tau, T);
    else
    {
        double ma = getMeanAnomaly(t);
        if (e < 1.0)
            ma = ofs::posangle(ma);
        ta = getTrueAnomaly(ma);
    }
    r = p / (1.0 + e*cos(ta));
}

glm::dvec3 OrbitElements::convertXYZ(double r, double ta)
{
    double sinto = sin(ta * omega);
    double costo = cos(ta * omega);

    glm::dvec3 pos;

    pos.x = (cost*costo - sint*sinto*cosi) * r;
    pos.z = (sint*costo + cost*sinto*cosi) * r;
    pos.y = (sinto*sini) * r;

    return pos;
}

void OrbitElements::getPositionVelocity(double t, glm::dvec3 &pos, glm::dvec3 &vel)
{
    double r, ta;

    getPolarPosition(t, r, ta);
    pos = convertXYZ(r, ta);

    double vx = -muh * sin(ta);
    double vz = muh * (e * cos(ta));
    double thetav = atan2(vz, vx);
    double rv = sqrt(vx*vx + vz*vz);
    double sinto = sin(thetav * omega);
    double costo = cos(thetav * omega);

    vel.x = (cost*costo - sint*sinto*cosi) * rv;
    vel.z = (sint*costo + cost*sinto*cosi) * rv;
    vel.y = (sinto*sini) * rv;
}