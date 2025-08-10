// elements.h - orbital elements package
//
// Author:  Tim Stark
// Date:    Aug 7, 2025

#pragma once

#define NELEMENTS   6
#define DEFAULT_ELEMENTS    { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

class OrbitalElements
{
public:
    OrbitalElements() = default;
    OrbitalElements(double a, double e, double i, 
        double theta, double omegab, double L0,
        double mjd = astro::MJD2000);
    OrbitalElements(const double *el, double mjd = astro::MJD2000);

    void setMasses(double m, double M);
    
    void reset(const double *el, double mjd);
    void configure(const double *el, double mjd = astro::MJD2000);
    void setup(double m, double M, double mjd);
    void start(double t, glm::dvec3 &pos, glm::dvec3 &vel);
    void update(double t, glm::dvec3 &pos, glm::dvec3 &vel);
    void determine(const glm::dvec3 &pos, const glm::dvec3 &vel, double t);

    inline double getLinearEccentricity() const             { return le; }
    inline double getApoapsisDistance() const               { return ad; }
    inline double getPeriapsisDistance() const              { return pd; }
    inline double getSemiMajorAxis() const                  { return a; }
    inline double getSemiMinorAxis() const                  { return b; }
    inline double getEccentricity() const                   { return e; }
    inline double getInclination() const                    { return i; }
    inline double getLongitudeOfAcendingNode() const        { return theta; }
    inline double getLongitudeOfPerapsis() const            { return omegab; }
    inline double getMeanLongitude() const                  { return L0; }
    inline double getOrbitalPeriod() const                  { return P; }

    inline double getMJDEpoch() const                       { return mjdEpoch; }

    inline double getRadiusVectorLength(double phi) const   { return p / (1.0 + e*cos(phi)); }
    inline glm::dvec3 getoPosition() const                  { return R; }
    inline glm::dvec3 getoVelocity() const                  { return V; }
    inline double getRadius() const                         { return r; }
    inline double getVelocity() const                       { return v; }

    inline double calculateMeanAnomaly(double t) const      { return n * (t-tau); }

    double calculateEccentricAnomaly(double ma);
    double calculateTrueAnomalyE(double ea);
    bool getAscendingNode(glm::dvec3 &an) const;
    bool getDescendingNode(glm::dvec3 &dn) const;

    void updatePolar(double t, double &r, double &ta);
    void convertPolarToXYZ(double r, double ta, glm::dvec3 &R);

public:
    // Primary orbital elements
    double a        = 1.0;  // Semi-major axis [m]
    double e        = 0.0;  // Eccentricity
    double i        = 0.0;  // Inclination [rad]
    double theta    = 0.0;  // Longitude of ascending node [rad]
    double omegab   = 0.0;  // Longitude of periapsis [rad]
    double L0       = 0.0;  // Mean longitude at epoch [rad]
    
    double sint, cost;  // sin/cos theta
    double sino, coso;  // sin/cos omega
    double sini, cosi;  // sin/cos inclination

protected:
    double mjdEpoch = astro::MJD2000;   // Epoch [mjd]
    double tEpoch;                      // Epoch [s]

    double P;       // orbital period [s]
    double Tap;     // time to next apoapsis passage [s]
    double Tpe;     // time to next periapsis passage [s]
    double tau;     // periapsis passage [s]

    double b;       // semi-minor axis [m]
    double le;      // linear eccentricity
    double ad;      // apoapsis distance
    double pd;      // periapsis distance
    double p;       // parameter of conic section
    double n;
    double h;
    double tmp;
    double omega;

    glm::dvec3 R;   // radius vector
    glm::dvec3 V;   // velocity vector
    double r;       // radius vector length
    double v;       // magnitude of velocity
    double ea;      // eccentric anomaly
    double ma;      // mean anomaly
    double tra;     // true anomaly
    double ml;      // mean longitude
    double trl;     // true longitude

    mutable double ma0 = 1e10;
    mutable double ea0 = 0.0;

    glm::dvec3 H;
    glm::dvec3 N;
    glm::dvec3 E;

    double m, M;    // two-body masses
    double mu;      // gravitional unit
    double muh;
};
