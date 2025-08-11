// elements.cpp - orbital elements package
//
// Author:  Tim Stark
// Date:    Aug 7, 2025

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/elements.h"

static const double E_CIRCLE_LIMIT = 1e-8;
static const double I_NOINC_LIMIT  = 1e-8;

OrbitalElements::OrbitalElements(double a, double e, double i,
    double theta, double omegab, double L0, double mjd)
: a(a), e(e), i(i), theta(theta), omegab(omegab), L0(L0),
  mjdEpoch(mjd)
{
    // set MJD date/time in seconds
    tEpoch = astro::seconds(mjd - ofsDate->getMJDReference());
}

OrbitalElements::OrbitalElements(const double *el, double mjd)
: a(el[0]), e(el[1]), i(el[2]), theta(el[3]), omegab(el[4]), L0(el[5]),
  mjdEpoch(mjd)
{
    // set MJD date/time in seconds
    tEpoch = astro::seconds(mjd - ofsDate->getMJDReference());
}

void OrbitalElements::configure(const double *el, double mjd)
{
    // Set primary orbital elements
    a = el[0];
    e = el[1];
    i = el[2];
    theta = el[3];
    omegab = el[4];
    L0 = el[5];

    // Set MJD date/time
    mjdEpoch = mjd;
    tEpoch = astro::seconds(mjd - ofsDate->getMJDReference());
}

void OrbitalElements::reset(const double *el, double mjd)
{

}

void OrbitalElements::setMasses(double _m, double _M)
{
    m = _m, M = _M;
    mu = astro::G * (m + M);
}

void OrbitalElements::updatePolar(double t, double &r, double &ta)
{
    if (e < E_CIRCLE_LIMIT)
        ta = n * fmod(t - tau, P);
    else {
        double ma = calculateMeanAnomaly(t);
        if (e < 1.0)
            ma = ofs::posangle(ma);
        ta = calculateTrueAnomalyE(ma);
    }

    r = p / (1.0 + e*cos(ta)); 
}

void OrbitalElements::convertPolarToXYZ(double r, double ta, glm::dvec3 &R)
{
    double sinto = sin(ta + omega);
    double costo = cos(ta + omega);
    R.x = r * (cost*costo - sint*sinto*cosi);
    R.z = r * (sint*costo + cost*sinto*cosi);
    R.y = r * (sinto*sini);
}

bool OrbitalElements::getAscendingNode(glm::dvec3 &an) const
{
    double d = getRadiusVectorLength(omega);
    an = N * d;
    return (d >= 0.0);
}

bool OrbitalElements::getDescendingNode(glm::dvec3 &dn) const
{
    double d = getRadiusVectorLength(omega);
    dn = N * -d;
    return (d >= 0.0);
}

double OrbitalElements::calculateEccentricAnomaly(double ma)
{
    constexpr const double tol = 1e-14;
    constexpr const int maxiter = 16;

    double res;
    double E;

    if (e < 1.0) {  // closed orbit
        E = fabs(ma-ma0) < 1e-2 ? ea0 : ma;
        res = ma - E + e * sin(E);
        if (fabs(res) > fabs(ma))
            E = 0.0, res = ma;
        for (int i = 0; fabs(res) > tol && i < maxiter; i++) {
            E += std::max(-1.0, std::min(1.0, res / (1.0 - e * cos(E))));
            res = ma - E + e * sin(E);
        }
    } else {        // open orbit
        E = ea0, res = 1.0;
        for (int i = 0; fabs(res) > tol && i < maxiter; i++) {
            E += std::max(-1.0, std::min(1.0, res / (e * cosh(E) - 1.0)));
            res = ma - e * sinh(E) + E;
        }
    }

    ma0 = ma;
    ea0 = E;
    return E;
}

double OrbitalElements::calculateTrueAnomalyE(double ea)
{
    if (e < 1.0) {      // closed orbit
        return 2.0 * atan(tmp * tan(0.5 * ea));
    } else {            // open orbit
        double coshea = cosh(ea);
        double tra = acos((e - coshea)/(e*coshea) - 1.0);
        return (ea >= 0.0) ? tra : -tra;
    }
}

void OrbitalElements::determine(const glm::dvec3 &pos, const glm::dvec3 &vel, double t)
{
    // Set initial position/velocity for determining orbital path
    R = pos * M_PER_KM;
    V = vel * M_PER_KM;

    double v2 = glm::length2(V);
    r = glm::length(R);
    v = sqrt(v2);
    if (r == 0 || v == 0)
        return;

    // semi-major axis
    a = r * mu / (2.0*mu - r*v2);

    // eccentricity
    double rv = glm::dot(R, V);
    E = (R * (1.0/r - 1.0/a) - V * (rv/mu));
    e = glm::length(E);

    // angular momentum
    H = glm::cross(R, V);
    h = glm::length(H);

    // inclination
    i = acos(H.z/h);
    sini = sin(i), cosi = cos(i);

    le = a * e;     // linear eccentricity
    pd = a - le;    // periapsis distance

    p = std::max(0.0, a * (1.0-e*e));   // parameter of conic section
    muh = sqrt(mu/p);                   // mu/h

    if (e < 1.0) {  // closed orbit
        ad = a + le;                    // apoapsis distance
        b = a * sqrt(1.0-e*e);          // semi-minor axis
        tmp = sqrt((1.0+e)/(1.0-e));    // for calculation of true anomaly
        n = sqrt(mu/(a*a*a));           // 2pi/T
        P = pi2/n;                      // orbital period
    } else {        // open orbit
        b = a * sqrt(e*e-1.0);          // semi-minor axis
        n = sqrt(mu/(-a*a*a));
    }

    // longitude of ascending node
    if (i > I_NOINC_LIMIT) {
        double tmp = 1.0/std::hypot(H.z, H.x);
        N = { -H.z*tmp, 0.0, H.x*tmp };
        theta = acos(N.x);
        if (N.z < 0.0)
            theta = pi2 - theta;
    } else {
        N = { 1.0, 0.0, 0.0 };
        theta = 0.0;
    }
    sint = sin(theta), cost = cos(theta);

    // argument of periapsis
    if (e > E_CIRCLE_LIMIT) {
        if (i > I_NOINC_LIMIT) {
            double arg = glm::dot(N, E)/e;
            if (arg < -1.0)     omega = pi;
            else if (arg > 1.0) omega = 0.0;
            else                omega = acos(arg);
            if (E.y < 0.0)
                omega = pi2 - omega;
        } else {
            if ((omega = atan2(E.z, E.x)) < 0.0)
                omega += pi2;
        }
    } else {
        omega = 0.0; // circular orbits
    }
    sino = sin(omega), coso = cos(omega);
    
    // longitude of periapsis
    if ((omegab = theta + omega) >= pi2)
        omegab -= pi2;

    // true anomaly
    if (e > E_CIRCLE_LIMIT) {
        tra = acos(std::clamp(glm::dot(E, R)/(e*r), -1.0, 1.0));
        if (rv < 0.0)
            tra = pi2-tra;
    } else {
        if (i > I_NOINC_LIMIT) {
            tra = acos(glm::dot(N, R)/r);
            if (glm::dot(N, V) > 0.0)
                tra = pi2-tra;
        } else {
            tra = acos(R.x/r);
            if (V.x > 0.0)
                tra = pi2-tra;
        }
    }

    // true longitude
    if ((trl = omegab + tra) >= pi2)
        trl -= pi2;
    if (e < 1.0) {  // closed orbit
        // eccentric anomaly
        if (e > 1e-8) {
            ea = atan2(rv*sqrt(a/mu), a-r);
        } else {
            ea = tra;
        }

        // mean anomaly
        ma = ea - e*sin(ea);

        // time of next periapsis and aposapsis passage
        if ((Tpe = -ma/n) < 0.0)
            Tpe += P;
        if ((Tap = Tpe-0.5*P) < 0.0)
            Tap += P;
    } else {    // open orbit
        // eccentric anomaly
        double costra = cos(tra);
        ea = acosh((e + costra) / (1.0 + e*costra));
        if (tra >= pi)
            ea = -ea;

        // mean anomaly
        ma = e*sinh(ea) - ea;

        // time of periapsis passage
        Tpe = -ma/n;
    }

    tau = t + Tpe;
    if (e < 1.0)
        tau -= P;
    
    // mean longitude at epoch
    if (e < 1.0) {  // closed orbit
        L0 = ofs::posangle(omegab + n * (tEpoch-tau));
        ml = ofs::posangle(ma + omegab);
    } else {
        L0 = omegab + n * (tEpoch-tau);
        ml = ma + omegab;
    }

    // ofsLogger->info("Orbital determination:\n");
    // ofsLogger->info("Position:            ({:.3f}, {:.3f}, {:.3f}) - {:.3f} km\n",
    //     pos.x, pos.y, pos.z, glm::length(pos));
    // ofsLogger->info("Velocity:            ({:.3f}, {:.3f}, {:.3f}) - {:.3f} mph\n",
    //     vel.x, vel.y, vel.z, glm::length(vel) * 3600 * 0.621);
    // ofsLogger->info("---- orbital elements ----\n");
    // ofsLogger->info("Semi-major Axis:     {:.2f} km\n", a / M_PER_KM);
    // ofsLogger->info("Semi-minor Axis:     {:.2f} km\n", b / M_PER_KM);
    // ofsLogger->info("Apoapsis:            {:.2f} km\n", ad / M_PER_KM);
    // ofsLogger->info("Periapsis:           {:.2f} km\n", pd / M_PER_KM);
    // ofsLogger->info("Eccentricity:        {:.6f}\n", e);
    // ofsLogger->info("Inclination:         {:.2f}\n", ofs::degrees(i));
    // ofsLogger->info("Long of asc node:    {:.2f}\n", ofs::degrees(theta));
    // ofsLogger->info("Long of perapsis:    {:.2f}\n", ofs::degrees(omegab));
    // ofsLogger->info("Mean longitude:      {:.2f}\n", ofs::degrees(L0));
    // ofsLogger->info("Orbital period:      {:.2f} mins ({:.1f} s)\n", P / 60.0, P);

}

void OrbitalElements::setup(double m, double M, double mjd)
{

    setMasses(m, M);

    le = a * e;                         // linear eccentricity
    ad = a + le;                        // apoapsis distance
    pd = a - le;                        // periapsis distance
    p = std::max(0.0, a * (1.0-e*e));   // parameter of conic section
    muh = sqrt(mu/p);                   // mu/h
    
    if (e < 1.0) {  // closed orbit
        b = a * sqrt(1.0 - e*e);            // semi-minor axis
        tmp = sqrt((1.0 + e)/(1.0 - e));    // for caculation of true anomaly
        n = sqrt(mu / (a*a*a));             // 2pi/T
    } else {        // open orbit
        b = a * sqrt(e*e - 1.0);
        n = sqrt(-mu / (a*a*a));
    }
    P = pi2 / n; // orbital period

    // Adjust mean longitude at start time
    L0 += astro::seconds(mjd - mjdEpoch) * n;
    mjdEpoch = mjd;
    tEpoch = astro::seconds(mjdEpoch - ofsDate->getMJDReference());
    tau = tEpoch - (L0 - omegab) / n;

    omega = omegab - theta;
    
    // Setup sin/cos parameters
    sint = sin(theta), cost = cos(theta);
    sini = sin(i),     cosi = cos(i);
    sino = sin(omega), coso = cos(omega);

    // angular momentum
    double h = sqrt(mu * p);    // magnitude of momentum
    H = { h*sini*sint, h*cosi, h*sini*cost };

    // ofsLogger->info("Apoapsis Distance:   {}\n", ad / M_PER_KM);
    // ofsLogger->info("Periapsis Distance:  {}\n", pd / M_PER_KM);
    // ofsLogger->info("Semi-major Axis:     {}\n", a / M_PER_KM);
    // ofsLogger->info("Semi-minor Axis:     {}\n", b / M_PER_KM);
    // ofsLogger->info("Orbital period:      {:.1f} ({:.2f} mins)\n", P, P / 60.0);
}

void OrbitalElements::start(double t, glm::dvec3 &pos, glm::dvec3 &vel)
{
    double sinto, costo;
    double vx, vz;
    double thetav;

    if (e < E_CIRCLE_LIMIT)
        ma = ea = tra = n * fmod(t - tau, P);
    else {
        ma = calculateMeanAnomaly(t);
        if (e < 1.0)
            ma = ofs::posangle(ma);
        ea = calculateEccentricAnomaly(ma);
        tra = calculateTrueAnomalyE(ea);
    }

    r = p / (1.0 + e*cos(tra));

    sinto = sin(tra + omega);
    costo = cos(tra + omega);
    R.x = r * (cost*costo - sint*sinto*cosi);
    R.y = r * (sint*costo + cost*sinto*cosi);
    R.z = r * (sinto*sini);
        
    vx = -muh * sin(tra);
    vz = muh * (e + cos(tra));
    thetav = atan2(vz, vx);
    v = sqrt(vx*vx + vz*vz); 

    sinto = sin(thetav + omega);
    costo = cos(thetav + omega);
    V.x = v * (cost*costo - sint*sinto*cosi);
    V.y = v * (sint*costo + cost*sinto*cosi);
    V.z = v * (sinto*sini);

    ml = ofs::posangle(ma + omegab);
    trl = ofs::posangle(tra + omegab);

    // Update time countdown for next
    // apoapsis/periapsis passage
    if ((Tpe = -ma/n) < 0.0)
        Tpe += P;
    if ((Tap = Tpe-0.5 * P) < 0.0)
        Tap += P;
    
    pos = R / M_PER_KM;
    vel = V / M_PER_KM;
}

void OrbitalElements::update(double t, glm::dvec3 &pos, glm::dvec3 &vel)
{
    double r, ta;
    double vx, vz, thetav;
    double sinto, costo;

    updatePolar(t, r, ta);
    convertPolarToXYZ(r, ta, R);

    vx = -muh * sin(ta);
    vz = muh * (e + cos(ta));
    thetav = atan2(vz, vx);
    v = sqrt(vx*vx + vz*vz); 

    sinto = sin(thetav + omega);
    costo = cos(thetav + omega);
    V.x = v * (cost*costo - sint*sinto*cosi);
    V.y = v * (sint*costo + cost*sinto*cosi);
    V.z = v * (sinto*sini);

    pos = R / M_PER_KM;
    vel = V / M_PER_KM;
}
