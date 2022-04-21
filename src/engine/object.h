// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Orbit;

class Object
{
public:
    enum ObjectType
    {
        objUnknown = 0,
        objPlayer,
        objVessel,
        objCelestialStar,
        objCelestialBody,
    };

    Object(const cstr_t &name, ObjectType type)
    : objType(type)
    {
        objNames[0] = name;
    }
    
    virtual ~Object() = default;

    inline void setsName(str_t name)    { objNames[0] = name; }    
    inline str_t getName() const        { return objNames[0]; }
    inline str_t getsName() const       { return objNames[0]; }
    inline ObjectType getType() const   { return objType; }
    inline double getRadius() const     { return radius; }
    inline Orbit *getOrbit() const      { return orbit; }

    inline void setMass(double val)     { mass = val; }
    inline void setRadius(double val)   { radius = val; }
    inline void setAlbedo(double val)   { albedo = val; }
    inline void setOrbit(Orbit *nOrbit) { orbit = nOrbit; }
    
private:
    ObjectType objType = objUnknown;
    std::vector<str_t> objNames{1};

protected:
    vec3d_t objPosition = { 0, 0, 0 };
    vec3d_t objVelocity = { 0, 0, 0 };
    quatd_t objRotation = { 1, 0, 0, 0 };


    double  mass   = 0.0;
    double  radius = 0.0;
    double  albedo = 0.0;

    Orbit *orbit = nullptr;
};