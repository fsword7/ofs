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
    cbSurfBase,
    cbStar,
    cbPlanet,
    cbDwarfPlanet,
    cbMoon,
    cbSubmoon,
    cbAsteroid,
    cbComet
};

enum frameType
{
    rfUniversal = 0,
    rfRelative,
    rfBodyFixed
};

using secondaries_t = const std::vector<Celestial *>;

class OFSAPI Celestial : public Object
{
public:
    Celestial(cstr_t &name, ObjectType type, celType ctype)
    : Object(name, type), cbType(ctype)
    { }

    Celestial(cjson &config, ObjectType type, celType ctype);
    virtual ~Celestial();

    void setup(cjson &config);
    void updateCelestial(bool force);

    inline celType getCelestialType() const { return cbType; }

    inline void setPlanetarySystem(pSystem *sys)        { system = sys; }
    inline pSystem *getPlanetarySystem() const          { return system; }
    inline void setSystem(pSystem *sys)                 { system = sys; }
    inline pSystem *getSystem() const                   { return system; }
    inline bool hasSystem() const                       { return system != nullptr; }

    inline void setEphemeris(OrbitEphemeris *ephem)     { ephemeris = ephem; }
    inline void setStar(Celestial *star)                { cstar = star; }

    inline bool hasSecondaries() const                  { return !secondaries.empty(); }
    inline int getSecondarySize() const                 { return secondaries.size(); }
    inline void addSecondary(Celestial *body)           { secondaries.push_back(body); }
    inline Celestial *getSecondary(int idx) const       { return idx < secondaries.size() ? secondaries[idx] : nullptr; }
    inline secondaries_t &getSecondaries() const        { return secondaries; }

    inline Celestial *getStar() const                   { return cstar; }
    inline Celestial *getParent() const                 { return cbody; }
    inline Celestial *getOrbitalReference() const       { return cbody; }

    inline void setVisualObject(vObject *vobj) const    { visualObject = vobj; }
    inline vObject *getVisualObject() const             { return visualObject; }

    inline bool isOrbitalValid() const                  { return bOrbitalValid; }
    inline const OrbitalElements &getOrbitalElements() const 
                                                        { return oel; }

    inline void flushPosition()                 { brpos = irpos, irpos = {}; }
    inline void flushVelocity()                 { brvel = irvel, irvel = {}; }
    
    inline double getRotationPeriod() const             { return rotT; }

    inline bool isSecondaryIlluminator() const          { return bIlluminator; }
    inline double getReflectivity() const               { return reflectivity; }

    void enableSecondaryIlluminator(bool enable)        { bIlluminator = enable; }
    void setReflectivity(double reflected)              { reflectivity = reflected; }

    void attach(Celestial *parent, frameType type = rfUniversal);

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
        return (gpos - s0.pos) * glm::transpose(s0.R);
    }

    inline glm::dvec3 convertGlobalToLocalS1(const glm::dvec3 &gpos) const
    {
        return (gpos - s1.pos) * glm::transpose(s1.R);
    }

    inline void convertGlobalToLocal(const glm::dvec3 &gpos, glm::dvec3 &lpos) const
    {
        lpos = (gpos - s0.pos) * glm::transpose(s0.R);
    }
    
    inline glm::dvec3 convertLocalToGlobal(const glm::dvec3 &lpos) const
    {
        return (lpos * s0.R) + s0.pos;
    }

    inline glm::dvec3 convertLocalToGlobalS1(const glm::dvec3 &lpos) const
    {
        return (lpos * s1.R) + s1.pos;
    }

    inline void convertLocalToEquatorial(const glm::dvec3 &lpos, double &lat, double &lng, double &rad) const
    {
        glm::dvec3 w = glm::normalize(lpos);

        rad = glm::length(lpos);
        lat = asin(w.y);
        lng = atan2(-w.z, w.x);
    }

    inline glm::dvec3 convertLocalToEquatorial(const glm::dvec3 &lpos) const
    {
        glm::dvec3 w = glm::normalize(lpos);
        return { asin(w.y), atan2(-w.z, w.x), glm::length(lpos) };
    }

    inline void convertGlobalToEquatorialS1(const glm::dvec3 &gpos, double &lat, double &lng, double &rad) const
    {
         return convertLocalToEquatorial(convertGlobalToLocal(gpos), lat, lng, rad);
    }

    inline glm::dvec3 convertGlobalToEquatorialS1(const glm::dvec3 &gpos) const
    {
        return convertLocalToEquatorial(convertGlobalToLocalS1(gpos));
    }

    inline glm::dvec3 convertEquatorialToLocal(double lat, double lng, double rad) const
    {
        double slat = sin(lat), clat = cos(lat);
        double slng = sin(lng), clng = cos(lng);
        double xz = rad * clat;

        return { xz*clng, rad*slat, xz*-slng };
    }

    inline glm::dvec3 convertEquatorialToLocal(double slat, double clat, double slng, double clng, double rad) const
    {
        double xz = rad * clat;
        return { xz*clng, rad*slat, xz*-slng };
    }

    inline glm::dvec3 convertEquatorialToLocal(const glm::dvec3 &epos) const
    {
        return convertEquatorialToLocal(epos.x, epos.y, epos.z);
        // double slat = sin(epos.x), clat = cos(epos.x);
        // double slng = sin(epos.y), clng = cos(epos.y);
        // double xz = epos.z * clat;

        // return { xz*clng, epos.z*slat, xz*-slng };
    }

    inline glm::dvec3 convertEquatorialToGlobal(double lat, double lng, double rad) const
    {
        return convertLocalToGlobal(convertEquatorialToLocal(lat, lng, rad));
    }

private:
    celType cbType = cbUnknown;

protected:
    pSystem *system = nullptr;
    Celestial *cstar = nullptr;
    Celestial *cbody = nullptr;    // orbit reference body

    mutable vObject   *visualObject = nullptr;

    OrbitalElements oel;
    bool bOrbitalValid = false;

    bool bIlluminator = false;
    double reflectivity = 0.5;

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
    bool bparent = false;

    // Rotation/prcession parameters
    double crot = 0.0;      // Current rotation
    double rotofs = 0.0;    // Rotation offset (precession)

    glm::dmat3 Recl;       // Precession matrix
    glm::dquat Qecl;       // Precession quaternion

private:

    // Precission/rotation perameters
    double      eps_ref;            // precession reference axis - obliquity against ecluptic normal
    double      lan_ref;            // precession reference axis - longitude of ascending node in ecliptic
    glm::dmat3  Rref;               // rotation matrix - ecliptic normal

    double      eps_ecl;            // obliquity of axis
    double      lan_ecl;            // longitude of ascending node

    double      eps_rel;            // obliquioty relavtive to reference axis
    double      cos_eps, sin_eps;   // sine/cosine of eps_rel

    double      mjd_rel;            // MJD epoch
    double      Lrel;               // longitude of ascending node relative to reference axis at current time
    double      Lrel0;              // longitude of ascending node relative to reference axis at MJD epoch
    double      precT;              // precission period (days) or 0 if infinite
    double      precOmega;          // precission angular velocity [rad/day]

    double      Dphi0 = 0.0;        // Rotation offset at t=0.
    double      Dphi = 0.0;         // Rotation offset at current time.
    double      rotT = 0.0;         // Rotation Period
    double      rotOmega = 0.0;     // Angular velocity

    glm::dvec3  Raxis;              // rotation axis (north pole) in global frame

};
