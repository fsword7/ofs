// body.h - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

#include "engine/object.h"

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
    
    inline void setStar(celStar *nStar)     { star = nStar; }

    void addBody(celBody *body);
    void removeBody(celBody *body);

    celBody *find(cstr_t &name) const;

private:
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

class celBody : public Object
{
public:
    enum {
        cbKnownRadius   = 1,
        cbKnownRotation = 2,
        cbKnownSurface  = 4,
        cbKnownObject   = (cbKnownRadius|cbKnownRotation|cbKnownSurface)
    };

    celBody(PlanetarySystem *system, cstr_t &name, celType type)
    : Object(name, objCelestialBody), cbType(type), inSystem(system)
    {
        inSystem->addBody(this);
    }

    celBody(cstr_t &name, celType type, celBody *body = nullptr)
    : Object(name, objCelestialBody), cbType(type)
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