// elements.h - orbital elements package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#pragma once

#define NELEMENTS           6
#define DEFAULT_ELEMENTS    { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

using orbit_t = double[NELEMENTS];

class OrbitalElements
{
public:
    OrbitalElements() = default;
    ~OrbitalElements() = default;

    OrbitalElements(double a, double e, double i, double theta,
        double omegab, double L, double mjd = astro::MJD2000);
    OrbitalElements(const double *el, double mjd = astro::MJD2000);

    inline bool isClosedOrbit(double e) const               { return e < 1.0; }

    inline double getLinearEccentricity() const             { return le; }
    inline double getApoapsis() const                       { return ad; }
    inline double getPeriapsis() const                      { return pd; }
    inline double getSemiMinorAxis() const                  { return b; }
    inline double getArgumentOfPeriapsis() const            { return omega; }
    inline double getOrbitalPeriod() const                  { return T; }

    inline double getMJDEpoch() const                       { return mjdEpoch; }

    inline double getCircularVelocity(double r) const       { return sqrt(mu / r); };
    inline double getRadiusVectorLength(double phi) const   { return p / (1.0 + e*cos(phi)); }

    inline double getMeanLongitude(double t) const          { return n*t + L; }
    inline double getMeanAnomaly(double t) const            { return n * (t-tau); }
    inline double getTrueAnomaly(double ma) const
        { return getTrueAnomalyE(getEccentricAnomaly(ma)); }

    inline glm::dvec3 getoPosition() const                  { return R; }
    inline glm::dvec3 getoVelocity() const                  { return V; }
    inline double getRadius() const                         { return r; }
    inline double getVelocity() const                       { return v; }

    void setMasses(double m, double M);
    
    double getEccentricAnomaly(double ma) const;
    double getEccentricAnomalyTA(double ta) const;
    double getTrueAnomalyE(double ea) const;
    double getMeanAnomalyE(double ea) const;
    bool getAscendingNode(glm::dvec3 &an) const;
    bool getDescendingNode(glm::dvec3 &dn) const;

    void configure(const double *el, double mjd);
    void reset(double a, double e, double i, double theta,
        double omegab, double L, double mjd);
    void reset(const double *el, double mjd);
    void setup(double m, double M, double mjd);
    void calculate(const glm::dvec3 &pos, const glm::dvec3 &vel, double t);
    void update(glm::dvec3 &pos, glm::dvec3 &vel, double t);

    void updatePolarPosition(double &r, double &ta, double t) const;
    glm::dvec3 convertPolarToXYZ(double r, double ta) const;
    glm::dvec3 getPosition(double t) const;
    void updateOrbiting(glm::dvec3 &pos, glm::dvec3 &vel, double t) const;

public:
    // Primary orbital element parameters
    double a      = 1.0;    // semi-major axis [m]
    double e      = 0.0;    // numerical eccentricity
    double i      = 0.0;    // inclination [rad]
    double theta  = 0.0;    // longitude of ascending node [rad]
    double omegab = 0.0;    // longitude of periapsis [rad]
    double L      = 0.0;    // mean longitude at epoch [rad]
    
    double sint, cost;  // sin/cos of theta
    double sini, cosi;  // sin/cos of i (inclination)
    double sino, coso;  // sin/cos of omega

private:
    // Secondary orbital element parameters
    double le;          // linear eccentricity [m]
    double ad;          // apoapsis distance [m]
    double pd;          // periapsis distance [m]
    double b;           // Semi-minor axis [m]
    double omega;       // Argument of periapsis [rad]
    double n;           // 2pi/T
    double tmp;         // calculation of true anomaly if e < 1

    double p;           // parameter of conic section

    // Mass parameters
    double m;           // mass of orbiter [kg]
    double M;           // mass of central object [kg]
    double mu;          // standard gravitational parameter G * (M+m)
    double muh;

    // Time parameters
    double T;           // orbital period [s]
    double Tpe;         // time to next periaspsis passage [s]
    double Tap;         // time to next apoaspsis passage [s]
    double tau;         // periapsis passage [s]

    double mjdEpoch;    // reference time (MJD format)
    double tEpoch;      // reference time

    // position parameters (only valid during time through update call)
    glm::dvec3 R;       // radius (position) vector
    glm::dvec3 V;       // velocity vector
    double     r;       // radius (position) vector length
    double     v;       // magnitude of velocity
    double     ea;      // eccentric anomaly
    double     ma;      // mean anomaly
    double     tra;     // true anomaly
    double     ml;      // mean longitude
    double     trl;     // true longitude
    
    glm::dvec3 H;       // normal to orbital plane
    glm::dvec3 E;       // points toward periapsis
    glm::dvec3 N;       // points toward ascending node

    mutable double ma0 = 1e10;
    mutable double ea0 = 0.0;
};