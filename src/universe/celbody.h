// body.h - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

#include "engine/rigidbody.h"

class CelestialStar;
class CelestialBody;
class OrbitEphemeris;
class pSystem;
class vObject;

// class PlanetarySystem
// {
// public:
//     PlanetarySystem(CelestialBody *body = nullptr);
//     PlanetarySystem(CelestialStar *star);
//     ~PlanetarySystem() = default;

//     inline CelestialBody *getPrimaryBody() const    { return body; }
//     inline CelestialStar *getStar() const           { return star; }
//     inline FrameTree *getSystemTree()               { return &tree; }

//     inline void setStar(CelestialStar *nStar)       { star = nStar; }

//     void addBody(CelestialBody *body);
//     void removeBody(CelestialBody *body);

//     CelestialBody *find(cstr_t &name) const;
    
// private:
//     FrameTree tree;

//     // Planetary system parameters
//     // System *solarSystem = nullptr;
//     CelestialBody *body = nullptr;
//     CelestialStar *star = nullptr;

//     std::vector<CelestialBody *> bodies;
// };


class OFSAPI CelestialBody : public RigidBody
{
public:
    enum {
        cbKnownRadius   = 1,
        cbKnownRotation = 2,
        cbKnownSurface  = 4,
        cbKnownObject   = (cbKnownRadius|cbKnownRotation|cbKnownSurface)
    };

    // CelestialBody(PlanetarySystem *system, cstr_t &name, celType type)
    // : RigidBody(name, (type == cbStar) ? objCelestialStar : objCelestialBody),
    //   cbType(type), inSystem(system)
    // {
    //     inSystem->addBody(this);
    // }

    CelestialBody(cstr_t &name, ObjectType type, celType cbtype)
    : RigidBody(name, type, cbtype)
    {
    }

    CelestialBody(json &config, ObjectType type, celType cbtype);
    virtual ~CelestialBody();

    // bool load(json &cfg);

    // inline void setStar(CelestialStar *star)            { cstar = star; }
    // inline CelestialStar *getStar() const               { return cstar; }
    // inline CelestialBody *getParent() const             { return cbody; }

    // inline double getRotationPeriod() const             { return rotT; }

    // inline celType getCelestialType() const { return cbType; }

    // inline void setPlanetarySystem(pSystem *psys)       { system = psys; }
    // inline void setEphemeris(OrbitEphemeris *ephem)     { ephemeris = ephem; }
    // inline void setInSystem(PlanetarySystem *system) { inSystem = system; }

    // inline PlanetarySystem *getOwnSystem() const    { return ownSystem; }
    // inline PlanetarySystem *getInSystem() const     { return inSystem; }
    // inline pSystem *getPlanetarySystem() const          { return system; }

    inline glm::dvec3 getcPosition() const          { return cpos; }
    inline glm::dvec3 getcVelocity() const          { return cvel; }

    inline void setColor(color_t nColor)            { color = nColor; }
    inline color_t getColor() const                 { return color; }

    inline void setPath(cstr_t &path)               { cbPath = path; }
    inline void setFolder(cstr_t &folder)           { cbFolder = folder; }
    inline str_t getPath() const                    { return cbPath; }
    inline str_t getFolder() const                  { return cbFolder; }

    // PlanetarySystem *createPlanetarySystem();
    
    glm::dmat3 getEquatorial(double tjd) const;

    double getLuminosity(double lum, double dist) const;
    double getApparentMagnitude(glm::dvec3 sun, double irradiance, glm::dvec3 view) const;

    glm::dmat3 getBodyFixedFromEcliptic(double tjd) const;

    glm::dvec3 getPlanetocentric(glm::dvec3 pos) const;
    glm::dvec3 getPlanetocentricFromEcliptic(const glm::dvec3 &pos, double tjd) const;
    glm::dvec3 getvPlanetocentricFromEcliptic(const glm::dvec3 &pos, double tjd) const;
    glm::dvec3 getHeliocentric(double tjd) const;
    
    // inline glm::dvec3 convertGlobalToLocal(const glm::dvec3 &gpos) const
    // {
    //     return glm::transpose(s0.R) * (gpos - s0.pos);
    // }

    // inline void convertGlobalToLocal(const glm::dvec3 &gpos, glm::dvec3 &lpos) const
    // {
    //     lpos = glm::transpose(s0.R) * (gpos - s0.pos);
    // }
    
    // inline glm::dvec3 convertLocalToGlobal(const glm::dvec3 &lpos) const
    // {
    //     return (s0.R * lpos) + s0.pos;
    // }

    // inline void convertLocalToEquatorial(const glm::dvec3 &lpos, double &lat, double &lng, double &rad) const
    // {
    //     glm::dvec3 w = glm::normalize(lpos);

    //     rad = glm::length(lpos);
    //     lat = asin(w.y);
    //     lng = atan2(-w.z, w.x);
    // }

    // inline void convertGlobalToEquatorial(const glm::dvec3 &gpos, double &lat, double &lng, double &rad)
    // {
    //      return convertLocalToEquatorial(convertGlobalToLocal(gpos), lat, lng, rad);
    // }

    // inline glm::dvec3 convertEquatorialToLocal(double lat, double lng, double rad)
    // {
    //     double slat = sin(lat), clat = cos(lat);
    //     double slng = sin(lng), clng = cos(lng);
    //     double xz = rad * clat;

    //     return { xz*clng, rad*slat, xz*-slng };
    // }

    // inline glm::dvec3 convertEquatorialToGlobal(double lat, double lng, double rad)
    // {
    //     return convertLocalToGlobal(convertEquatorialToLocal(lat, lng, rad));
    // }

// private:
//     celType cbType = cbUnknown;

protected:
    color_t color = color_t(0.7f, 0.7f, 0.7f, 1.0f);
    uint32_t knownFlags = 0;

    str_t cbPath;
    str_t cbFolder;

    // pSystem *system = nullptr;
    // OrbitEphemeris *ephemeris = nullptr;

    // CelestialStar *cstar = nullptr;
    // CelestialBody *cbody = nullptr;             // Reference frame for orbits
    // std::vector<CelestialBody *> secondaries;   // children of celstial body

    // mutable vObject *visualObject = nullptr;

    // // Rotation/prcession parameters
    // double crot = 0.0;      // Current rotation
    // double rotofs = 0.0;    // Rotation offset (precession)

    // glm::dmat3 R_ref_rel;   // rotation matrix
    // glm::dmat3 Recl;    // Precession matrix
    // glm::dquat Qecl;    // Precession quaternion

    // // Ephemeris data parameters (orbital frame)
    // glm::dvec3 cpos;    // orbital position
    // glm::dvec3 cvel;    // orbital velocity

    // // Barycentre data parameters (orbital reference frame)
    // glm::dvec3 bpos;    // barycentre position
    // glm::dvec3 bvel;    // barycentre velocity
    // glm::dvec3 bposofs; // barycentre position offset (reference frame)
    // glm::dvec3 bvelofs; // barycentre velocity offset (reference frame)
    // bool bparent;

    // celSurface surface;

    // PlanetarySystem *ownSystem = nullptr;
    // PlanetarySystem *inSystem = nullptr;

// private:

//     // Precission/rotation perameters
//     double      eps_ref;            // precession reference axis - obliquity against ecluptic normal
//     double      lan_ref;            // precession reference axis - longitude of ascending node in ecliptic
//     glm::dmat3  R_ref;              // rotation matrix - ecliptic normal

//     double      eps_ecl;            // obliquity of axis
//     double      lan_ecl;            // longitude of ascending node

//     double      eps_rel;            // obliquioty relavtive to reference axis
//     double      cos_eps, sin_eps;   // sine/cosine of eps_rel

//     double      mjd_rel;            // MJD epoch
//     double      Lrel;               // longitude of ascending node relative to reference axis at current time
//     double      Lrel0;              // longitude of ascending node relative to reference axis at MJD epoch
//     double      precT;              // precission period (days) or 0 if infinite
//     double      precOmega;          // precission angular velocity [rad/day]

//     double      Dphi = 0.0;         // Rotation offset at t=0.
//     double      rotT = 0.0;         // Rotation Period
//     double      rotOmega = 0.0;     // Angular velocity
//     double      rotOffset = 0.0;    // 
//     glm::dvec3  Raxis;              // rotation axis (north pole) in global frame

};