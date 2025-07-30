// elements.cpp - orbital elements package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/elements.h"

static constexpr const double E_CIRCLE_LIMIT = 1e-8;
static constexpr const double I_NOINC_LIMIT  = 1e-8;

OrbitalElements::OrbitalElements(double a, double e, double i,
    double theta, double omegab, double L, double mjd)
: a(a), e(e), i(i), theta(theta), omegab(omegab), L(L),
  mjdEpoch(mjd)
{ 
    tEpoch = (mjdEpoch - ofsDate->getMJDReference()) * 86400.0;
}

OrbitalElements::OrbitalElements(const double *el, double mjd)
: a(el[0]), e(el[1]), i(el[2]), theta(el[3]),
  omegab(el[4]), L(el[5]), mjdEpoch(mjd)
{
    tEpoch = (mjdEpoch - ofsDate->getMJDReference()) * 86400.0;
}

void OrbitalElements::configure(const double *el, double mjd)
{
    // Set new orbital elmenets
    a = el[0];
    e = el[1];
    i = el[2];
    theta = el[3];
    omegab = el[4];
    L = el[5];
    ma = 1e10;

    // Set MJD date/time
    mjdEpoch = mjd;
    tEpoch = (mjdEpoch - ofsDate->getMJDReference()) * 86400.0;
}

void OrbitalElements::reset(double _a, double _e, double _i, double _theta,
    double _omegab, double _L, double mjd)
{
    // Reset primary elements
    a = _a;
    e = _e;
    i = _i;
    theta = _theta;
    omegab = _omegab;
    L = _L;

    // Reset MJD date/time
    setup(m, M, mjd);
}

void OrbitalElements::reset(const double *el, double mjd)
{
    // Reset primary elements
    a = el[0];
    e = el[1];
    i = el[2];
    theta = el[3];
    omegab = el[4];
    L = el[5];

    // Reset MJD date/time
    setup(m, M, mjd);
}

void OrbitalElements::setMasses(double _m, double _M)
{
    m  = _m;
    M  = _M;
    mu = astro::G * (m + M);
}

double OrbitalElements::getEccentricAnomaly(double ma) const
{

    constexpr const double tol = 1e-16;
    constexpr const int maxiter = 16;

    double res;
    double E;

    if (isClosedOrbit(e))
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

double OrbitalElements::getEccentricAnomalyTA(double ta) const
{
    if (isClosedOrbit(e))
        return 2.0 * atan(tan(0.5 * ta) / tmp);
    else
    {
        double costa = cos(ta);
        double ea = acosh((e*costa)/(e*costa+1.0));
        return (ta >= 0.0) ? ea : -ea;
    }
}

double OrbitalElements::getTrueAnomalyE(double ea) const
{
    if (isClosedOrbit(e))
        return 2.0 * atan(tmp * tan(0.5 * ea));
    else
    {
        double coshea = cosh(ea);
        double tra = acos((e - coshea)/(e*coshea - 1.0));
        return (ea >= 0.0) ? tra : -tra;
    }
}

double OrbitalElements::getMeanAnomalyE(double ea) const
{
    if (isClosedOrbit(e))
        return ea - e*sin(ea);
    else
        return e*sinh(ea) - ea;
}

bool OrbitalElements::getAscendingNode(glm::dvec3 &an) const
{
    double d = getRadiusVectorLength(omega);
    an = N * d;
    return (d >= 0.0);
}

bool OrbitalElements::getDescendingNode(glm::dvec3 &dn) const
{
    double d = getRadiusVectorLength(omega+pi);
    dn = N * -d;
    return (d >= 0.0);
}


// determine orbital data (free fall) by using masses
void OrbitalElements::setup(double m, double M, double mjd)
{
    setMasses(m, M);

    le = a * e;     // linear eccentricity
    ad = a + le;    // apoapsis distance
    pd = a - le;    // periapsis distance
    omega = omegab - theta;

    p = std::max(0.0, a * (1.0 - a*a));
    muh = sqrt(mu/p);

    if (isClosedOrbit(e))
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
    T = (pi * 2.0) / n;                  // orbital period
    L += (mjd - mjdEpoch) * 86400.0 * n; // mean longitude
    mjdEpoch = mjd;
    tEpoch = (mjdEpoch - ofsDate->getMJDReference()) * 86400.0;
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

    a = r * mu / (2.0 * mu - r * glm::length2(V));

    H = glm::cross(V, R);
    double h = glm::length(H);
    double rv = glm::dot(R, V);

    E = (R * 1.0/r - 1.0/a) - V * (rv / mu);
    e = glm::length(E);

    // inclination
    i = acos(H.y/h);

    sini = sin(i), cosi = cos(i);

    le = a * e;
    pd = a - le;
    p = std::max(0.0, a * (1.0 - e*e));
    muh = sqrt(mu/p);

    if (isClosedOrbit(e))
    {
        ad = a + le;
        b = a * sqrt(1.0 - e*e);
        tmp = sqrt(1.0 + e) / (1.0 - e);
        n = sqrt(mu / (a*a*a));
        T = (pi*2)/n;
    }
    else
    {
        b = a * sqrt(e*e - 1.0);
        n = sqrt(mu * (-a*a*a));
    }

    // Longitude of ascending node
    if (i > I_NOINC_LIMIT)
    {
        double tmp = 1.0 / std::hypot(H.z, H.x);
        N = { -H.z*tmp, 0.0, H.x*tmp };
        theta = acos(N.x);
        if (N.z < 0.0)
            theta = (pi*2) - theta;
    }
    else
    {
        N = { 0, 0, 0 };
        theta = 0.0;
    }
    sint = sin(theta), cost = cos(theta);

    // Argument of periapsis
    if (e > E_CIRCLE_LIMIT)
    {
        if (i > I_NOINC_LIMIT)
        {
            double arg = glm::dot(N, E) / e;
            omega = (arg < -1.0) ? omega = pi :
                    (arg >  1.0) ? omega = 0.0 :
                                   omega = acos(arg);
            if (E.y < 0.0)
                omega = (pi*2) - omega;
        }
        else
        {
            if ((omega = atan2(E.z, E.x)) < 0.0)
                omega += pi*2;
        }
    } else
        omega = 0.0;
    sino = sin(omega), coso = cos(omega);

    // Longitude of periapsis
    if ((omegab = theta + omega) >= pi*2)
        omegab -= pi*2;

    // True anomaly
    if (e > E_CIRCLE_LIMIT)
    {
        tra = acos(std::clamp(glm::dot(E, R) / (e * r), -1.0, 1.0));
        if (rv < 0.0)
            tra = (pi*2) - tra;
    }
    else
    {
        if (i > I_NOINC_LIMIT)
        {
            tra = acos(glm::dot(N, R)/r);
            if (glm::dot(N, V) > 0.0)
                tra = (pi*2) - tra;
        }
        else
        {
            tra = acos(R.x/r);
            if (V.x > 0.0)
                tra = (pi*2) - tra;
        }
    }

    // True longitude
    if ((trl = omegab + tra) >= pi*2)
        trl -= pi*2;
    if (isClosedOrbit(e))
    {
        // Closed orbit

        // eccentric anomaly
        if (e > E_CIRCLE_LIMIT)
            ea = atan2(rv*sqrt(a/mu), a-r);
        else
            ea = tra;

        // mean anomaly
        ma = ea - e*sin(ea);

        // time of next periapsis amd apoapsis passage
        if ((Tpe = -ma/n) < 0.0)
            Tpe += T;
        if ((Tap = Tpe-0.5*T) < 0.0)
            Tap += T;
    }
    else
    {
        // Open orbit

        // Eccentric anomaly
        double costra = cos(tra);
        ea = acosh((e + costra)/(1.0 + e * costra));
        if (tra >= pi)
            ea = -ea;

        // mean anomaly
        ma = e * sinh(ea) - ea;

        // time of periapsis passage
        Tpe = -ma/n;
    }

    // time of periapsis passage
    tau = t + Tpe;
    if (isClosedOrbit(e))
        tau -= T;
    
    // mean longitude at epoch
    if (isClosedOrbit(e))
    {
        L = ofs::posangle(omegab + n * (tEpoch - tau));
        ml = ofs::posangle(ma + omegab);
    }
    else
    {
        L = omegab + n * (tEpoch-tau);
        ml = ma + omegab;
    }
}

// updating position and velocity periodically
void OrbitalElements::update(double mjd, glm::dvec3 &pos, glm::dvec3 &vel)
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

    ml = ofs::posangle(ma + omegab);
    trl = ofs::posangle(tra + omegab);

    // Update time countdown for next
    // periaspsis/apoaspsis passage
    if ((Tpe = -ma/n) < 0.0)
        Tpe += T;
    if ((Tap = Tpe-0.5 * T) < 0.0)
        Tap += T;

    // Return new position/velocity
    pos = R;
    vel = V;
}

glm::dvec3 OrbitalElements::convertPolarToXYZ(double r, double ta) const
{
    double sinto = sin(ta * omega);
    double costo = cos(ta * omega);

    glm::dvec3 pos;

    pos.x = (cost*costo - sint*sinto*cosi) * r;
    pos.z = (sint*costo + cost*sinto*cosi) * r;
    pos.y = (sinto*sini) * r;

    return pos;
}

void OrbitalElements::getPolarPosition(double t, double &r, double &ta) const
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

glm::dvec3 OrbitalElements::getPosition(double t) const
{
    glm::dvec3 pos;
    double r, ta;

    getPolarPosition(t, r, ta);
    pos = convertPolarToXYZ(r, ta);

    return pos;
}

void OrbitalElements::getPositionVelocity(double t, glm::dvec3 &pos, glm::dvec3 &vel) const
{
    double r, ta;

    getPolarPosition(t, r, ta);
    pos = convertPolarToXYZ(r, ta);

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