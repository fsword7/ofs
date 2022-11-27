// body.h - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

#include "engine/rigidbody.h"
#include "universe/frame.h"
#include "universe/surface.h"

class CelestialStar;
class CelestialBody;

class PlanetarySystem
{
public:
    PlanetarySystem(CelestialBody *body = nullptr);
    PlanetarySystem(CelestialStar *star);
    ~PlanetarySystem() = default;

    inline CelestialBody *getPrimaryBody() const    { return body; }
    inline CelestialStar *getStar() const           { return star; }
    inline FrameTree *getSystemTree()               { return &tree; }

    inline void setStar(CelestialStar *nStar)       { star = nStar; }

    void addBody(CelestialBody *body);
    void removeBody(CelestialBody *body);

    CelestialBody *find(cstr_t &name) const;
    
private:
    FrameTree tree;

    // Planetary system parameters
    // System *solarSystem = nullptr;
    CelestialBody *body = nullptr;
    CelestialStar *star = nullptr;

    std::vector<CelestialBody *> bodies;
};

enum celType
{
    cbUnknown = 0,
    cbStar,
    cbPlanet,
    cbDwarfPlanet,
    cbMoon,
    cbSubmoon,
    cbAsteroid,
    cbComet
};

class CelestialBody : public RigidBody
{
public:
    enum {
        cbKnownRadius   = 1,
        cbKnownRotation = 2,
        cbKnownSurface  = 4,
        cbKnownObject   = (cbKnownRadius|cbKnownRotation|cbKnownSurface)
    };

    CelestialBody(PlanetarySystem *system, cstr_t &name, celType type)
    : RigidBody(name, (type == cbStar) ? objCelestialStar : objCelestialBody),
      cbType(type), inSystem(system)
    {
        inSystem->addBody(this);
    }

    CelestialBody(cstr_t &name, celType type, CelestialBody *body = nullptr)
    : RigidBody(name, (type == cbStar) ? objCelestialStar : objCelestialBody),
      cbType(type)
    {
        if (body != nullptr)
        {
            inSystem = body->createPlanetarySystem();
            inSystem->addBody(this);
        }
    }

    virtual ~CelestialBody() = default;

    virtual void update(bool force);

    inline celType getCelestialType() const { return cbType; }

    inline void setInSystem(PlanetarySystem *system) { inSystem = system; }

    inline PlanetarySystem *getOwnSystem() const    { return ownSystem; }
    inline PlanetarySystem *getInSystem() const     { return inSystem; }

    inline void setColor(color_t nColor)            { color = nColor; }
    inline color_t getColor() const                 { return color; }

    PlanetarySystem *createPlanetarySystem();
    
    glm::dmat3 getEquatorial(double tjd) const;

    double getLuminosity(double lum, double dist) const;
    double getApparentMagnitude(glm::dvec3 sun, double irradiance, glm::dvec3 view) const;

    glm::dmat3 getBodyFixedFromEcliptic(double tjd) const;

    glm::dvec3 getPlanetocentric(glm::dvec3 pos) const;
    glm::dvec3 getPlanetocentricFromEcliptic(const glm::dvec3 &pos, double tjd) const;
    glm::dvec3 getvPlanetocentricFromEcliptic(const glm::dvec3 &pos, double tjd) const;
    glm::dvec3 getHeliocentric(double tjd) const;

private:
    celType cbType = cbUnknown;

protected:
    color_t color = color_t(0.7f, 0.7f, 0.7f, 1.0f);
    uint32_t knownFlags = 0;

    celSurface surface;

    PlanetarySystem *ownSystem = nullptr;
    PlanetarySystem *inSystem = nullptr;
};