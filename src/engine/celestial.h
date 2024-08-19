// celestial.h - Celestial Object package
//
// Author:  Tim Stark
// Date:    Aug 4, 2024

#pragma once

#include "engine/object.h"
#include "ephem/elements.h"

class pSystem;
class Celestial;
class OrbitEphemeris;
class vObject;

enum celType
{
    cbUnknown = 0,
    cbObserver,
    cbVehicle,
    cbStar,
    cbPlanet,
    cbDwarfPlanet,
    cbMoon,
    cbSubmoon,
    cbAsteroid,
    cbComet
};

using secondaries_t = const std::vector<Celestial *>;

class OFSAPI Celestial : public Object
{
public:
    Celestial(cstr_t &name, ObjectType type, celType ctype)
    : Object(name, type), cbType(ctype)
    { }

    Celestial(YAML::Node &config, ObjectType type, celType ctype);
    virtual ~Celestial();

    void setup(YAML::Node &config);

    void update(bool force);

    inline celType getCelestialType() const { return cbType; }

    inline void setPlanetarySystem(pSystem *sys)        { system = sys; }
    inline pSystem *getPlanetarySystem() const          { return system; }
    inline void setEphemeris(OrbitEphemeris *ephem)     { ephemeris = ephem; }

    inline void addSecondary(Celestial *body)           { secondaries.push_back(body); }
    inline secondaries_t &getSecondaries() const        { return secondaries; }

    inline void setVisualObject(vObject *vobj) const    { visualObject = vobj; }
    inline vObject *getVisualObject() const             { return visualObject; }

    inline void flushPosition()                 { brpos = irpos, irpos = {}; }
    inline void flushVelocity()                 { brvel = irvel, irvel = {}; }
    
    inline double getRotationPeriod() const             { return rotT; }

    StateVectors interpolateState(double step);
    glm::dvec3 interpolatePosition(double step) const;

    void convertPolarToXYZ(double *pol, double *xyz, bool hpos, bool hvel);
    uint32_t getEphemerisState(double *res);
    bool updateEphemeris();
    void updatePostEphemeris();

    void updatePrecission();
    void setupRotation();
    glm::dmat3 getRotation(double t) const;
    void updateRotation();


    inline glm::dvec3 convertGlobalToLocal(const glm::dvec3 &gpos) const
    {
        return glm::transpose(s0.R) * (gpos - s0.pos);
    }

    inline void convertGlobalToLocal(const glm::dvec3 &gpos, glm::dvec3 &lpos) const
    {
        lpos = glm::transpose(s0.R) * (gpos - s0.pos);
    }
    
    inline glm::dvec3 convertLocalToGlobal(const glm::dvec3 &lpos) const
    {
        return (s0.R * lpos) + s0.pos;
    }

    inline void convertLocalToEquatorial(const glm::dvec3 &lpos, double &lat, double &lng, double &rad) const
    {
        glm::dvec3 w = glm::normalize(lpos);

        rad = glm::length(lpos);
        lat = asin(w.y);
        lng = atan2(-w.z, w.x);
    }

private:
    celType cbType = cbUnknown;

protected:
    pSystem *system = nullptr;
    Celestial *cbody = nullptr;    // orbit reference body
    
    mutable vObject   *visualObject = nullptr;

    OrbitalElements oel;
    bool orbitValid = false;

    OrbitEphemeris *ephemeris = nullptr;

    // CelestialStar *cstar = nullptr;
    // CelestialBody *cbody = nullptr;             // Reference frame for orbits
    std::vector<Celestial *> secondaries;   // children of celstial body

    glm::dvec3 brpos, irpos;    // relative position (base and incremental)
    glm::dvec3 brvel, irvel;    // relative velocity (base and incremantel)

    // Ephemeris data parameters (orbital frame)
    glm::dvec3 cpos;    // orbital position
    glm::dvec3 cvel;    // orbital velocity

    // Barycentre data parameters (orbital reference frame)
    glm::dvec3 bpos;    // barycentre position
    glm::dvec3 bvel;    // barycentre velocity
    glm::dvec3 bposofs; // barycentre position offset (reference frame)
    glm::dvec3 bvelofs; // barycentre velocity offset (reference frame)
    bool bparent;

    // Rotation/prcession parameters
    double crot = 0.0;      // Current rotation
    double rotofs = 0.0;    // Rotation offset (precession)

    glm::dmat3 R_ref_rel;   // rotation matrix
    glm::dmat3 Recl;    // Precession matrix
    glm::dquat Qecl;    // Precession quaternion

private:

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
    double      rotT = 0.0;         // Rotation Period
    double      rotOmega = 0.0;     // Angular velocity
    double      rotOffset = 0.0;    // 
    glm::dvec3  Raxis;              // rotation axis (north pole) in global frame

};
