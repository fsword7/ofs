// system.h - solar system package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#pragma once

class celStar;
class Universe;
class Parser;
class Value;
class Group;

#include "universe/body.h"

class System
{
public:
    System(celStar *star);
    ~System() = default;

    inline PlanetarySystem *getPlanetarySystem()    { return &objects; }

    static celBody *createBody(cstr_t &name, PlanetarySystem *system,
        celType type, cstr_t &orbitFrameName, cstr_t &bodyFrameName);

    // Loading SSO system file
    static bool logError(const Parser &parser, cstr_t &message);
    static void initClassifications();
    static celType getClassification(cstr_t &className);
    static Object *getFrameCenter(Universe &universe, Group *gFrame, Object *defaultObject);
    static Frame *createJ2000EclipticFrame(Universe &universe, Group *vFrame, Object *center);
    static Frame *createJ2000EquatorFrame(Universe &universe, Group *vFrame, Object *center);
    static Frame *createReferenceFrame(Universe &universe, Group *gFrame, Object *center, Object *body);
    static Frame *createReferenceFrame(Universe &universe, Value *vFrame, Object *center, Object *body);
    static Orbit *createOrbit(Object *centerObject, Group *objData, const fs::path &path);
    static Rotation *createRotation(Object *centerObject, Group *objData, const fs::path &path);
    static void setSurface(celSurface &surface, Group *objData);
    static celBody *createBody2(cstr_t &name, celType type, PlanetarySystem *pSystem,
        Universe &universe, Group *objData);
    static bool loadSolarSystemObjects(std::istream &in, Universe &universe, const fs::path &path);

private:
    std::vector<celStar *> stars; // Multi-star systems
    PlanetarySystem objects;
};

typedef std::map<uint32_t, System *> SystemsList;