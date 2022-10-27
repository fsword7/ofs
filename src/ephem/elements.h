// elements.h - orbital elements package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#pragma once

class OrbitalElements
{
public:
    OrbitalElements() = default;
    ~OrbitalElements() = default;

    OrbitalElements(double a, double e, double i, double theta, double omegab, double L, double mjd);

    inline double getLinearEccentricity() const  { return le; }
    inline double getApoapsis() const            { return ad; }
    inline double getPeriapsis() const           { return pd; }
    inline double getSemiMinorAxis() const       { return b; }
    inline double getArgumentOfPeriapsis() const { return omega; }
    inline double getOrbitalPeriod() const       { return T; }

    inline double getMJDEpoch() const            { return mjdEpoch; }
    
    void setMasses(double m, double M);
    double getCircularVelocity(double r) const   { return sqrt(mu / r); };

    void setup(double m, double M, double mjd);
    void calculate(const glm::dvec3 &pos, const glm::dvec3 &vel, double t);
    void update(glm::dvec3 &pos, glm::dvec3 &vel);

public:
    // Primary orbital element parameters
    double a      = 1.0;    // semi-major axis [m]
    double e      = 0.0;    // numerical eccentricity
    double i      = 0.0;    // inclination [rad]
    double theta  = 0.0;    // longitude of ascending node [rad]
    double omegab = 0.0;    // longitude of periapsis [rad]
    double L      = 0.0;    // mean longitude at epoch

private:
    // Secondary orbital element parameters
    double le;          // linear eccentricity [m]
    double ad;          // apoapsis distance [m]
    double pd;          // periapsis distance [m]
    double b;           // Semi-minor axis [m]
    double omega;       // Argument of periapsis [rad]
    double n;           // 2pi/T
    double tmp;         // calculation of true anomaly if e < 1

    // Mass parameters
    double m;           // mass of orbiter [kg]
    double M;           // mass of central object [kg]
    double mu;          // standard gravitational parameter G * (M+m)

    // Time parameters
    double T;           // orbital period [s]
    double Tpe;         // time to next periaspsis passage [s]
    double Tap;         // time to bext apoaspsis passage [s]
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
};