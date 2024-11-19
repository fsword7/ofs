// rotation.h - Planet Precession/Rotation Package
//
// Author:  Tim Stark
// Date:    Apr 15, 2024

#pragma once

class Rotation
{
public:
    Rotation() = default;
    virtual ~Rotation() = default;

    static Rotation *create(YAML::Node &config);

    virtual void update(double mjd) = 0;
};

class RotationFixed : public Rotation
{
public:
    RotationFixed() = default;
    virtual ~RotationFixed() = default;

};

class RotationUniform : public Rotation
{
public:
    RotationUniform() = default;
    RotationUniform(YAML::Node &config);
    virtual ~RotationUniform() = default;

    void configure(YAML::Node &config);

    void update(double mjd) override;
    double spin(double mjd);
    
private:
    double crot    = 0.0;
    double crotofs = 0.0;

    // Precission/rotation perameters
    double      eps_ref;            // precession reference axis - obliquity against ecluptic normal
    double      lan_ref;            // precession reference axis - longitude of ascending node in ecliptic
    glm::dmat3  R_ref;              // rotation matrix - ecliptic normal

    double      eps_ecl;            // obliquity of axis
    double      lan_ecl;            // longitude of ascending node

    double      eps_rel;            // obliquioty relavtive to reference axis
    double      cos_eps, sin_eps;   // sine/cosine of eps_rel

    double      mjd_rel;            // MJD epoch
    double      Lrel;               // longitude of ascending node relative to reference axis at current time
    double      Lrel0;              // longitude of ascending node relative to reference axis at MJD epoch
    double      precT;              // precission period (days) or 0 if infinite
    double      precOmega;          // precission angular velocity [rad/day]

    double      Dphi = 0.0;         // Rotation offset at t=0.
    double      rotT = 0.0;         // sideral rotation - Time
    double      rotOmega = 0.0;     // sideral rotation - Angular velocity
    double      rotOffset = 0.0;    // 
    glm::dvec3  Raxis;              // rotation axis (north pole) in global frame

    glm::dmat3 R_ref_rel;   // rotation matrix
    glm::dmat3 Recl;    // Precession matrix
    glm::dquat Qecl;    // Precession quaternion
};