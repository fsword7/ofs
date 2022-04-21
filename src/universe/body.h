// body.h - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

#include "engine/rigidbody.h"
#include "universe/frame.h"

class celStar;
class celBody;

class PlanetarySystem
{
public:
    PlanetarySystem(celBody *body = nullptr);
    PlanetarySystem(celStar *star);
    ~PlanetarySystem() = default;

    inline celBody *getPrimaryBody()        { return body; }
    inline celStar *getStar()               { return star; }
    inline FrameTree *getSystemTree()       { return &tree; }

    inline void setStar(celStar *nStar)     { star = nStar; }

    void addBody(celBody *body);
    void removeBody(celBody *body);

    celBody *find(cstr_t &name) const;

private:
    FrameTree tree;

    // Planetary system parameters
    // System *solarSystem = nullptr;
    celBody *body = nullptr;
    celStar *star = nullptr;

    std::vector<celBody *> bodies;
};

enum celType
{
    cbUnknown = 0,
    cbStar,
    cbPlanet,
    cbDwarfPlanet,
    cbMoon,
    cbAsteroid,
    cbComet
};

class celBody : public RigidBody
{
public:
    enum {
        cbKnownRadius   = 1,
        cbKnownRotation = 2,
        cbKnownSurface  = 4,
        cbKnownObject   = (cbKnownRadius|cbKnownRotation|cbKnownSurface)
    };

    celBody(PlanetarySystem *system, cstr_t &name, celType type)
    : RigidBody(name, objCelestialBody), cbType(type), inSystem(system)
    {
        inSystem->addBody(this);
    }

    celBody(cstr_t &name, celType type, celBody *body = nullptr)
    : RigidBody(name, objCelestialBody), cbType(type)
    {
        if (body != nullptr)
        {
            inSystem = body->createPlanetarySystem();
            inSystem->addBody(this);
        }
    }

    virtual ~celBody() = default;

    inline celType getCelestialType() const { return cbType; }

    inline void setInSystem(PlanetarySystem *system) { inSystem = system; }

    inline PlanetarySystem *getOwnSystem() const    { return ownSystem; }
    inline PlanetarySystem *getInSystem() const     { return inSystem; }

    inline void setColor(color_t nColor)            { color = nColor; }
    inline color_t getColor() const                 { return color; }

    PlanetarySystem *createPlanetarySystem();

    double getLuminosity(double lum, double dist) const;
    double getApparentMagnitude(vec3d_t sun, double irradiance, vec3d_t view) const;
    
private:
    celType cbType = cbUnknown;

protected:
    color_t color = color_t(0.7f, 0.7f, 0.7f, 1.0f);
    uint32_t knownFlags = 0;

    PlanetarySystem *ownSystem = nullptr;
    PlanetarySystem *inSystem = nullptr;
};