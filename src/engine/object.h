// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Orbit;
class Rotation;
class Frame;

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

    inline void setMass(double val)     { mass = val; }
    inline void setRadius(double val)   { radius = val; }
    inline void setAlbedo(double val)   { albedo = val; }

    // Virtual function calls
    virtual void setOrbitFrame(Frame *frame) = 0;
    virtual void setBodyFrame(Frame *frame) = 0;
    virtual void setOrbit(Orbit *nOrbit) = 0;
    virtual void setRotation(Rotation *nRotation) = 0;

    virtual Frame *getOrbitFrame() const = 0;
    virtual Frame *getBodyFrame() const = 0;
    virtual Orbit *getOrbit() const = 0;
    virtual Rotation *getRotation() const = 0;

    virtual vec3d_t getuPosition(double tjd) const;
    virtual quatd_t getuOrientation(double tjd) const;
    virtual vec3d_t getoPosition(double tjd) const;

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
};