// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#include "render/refmark.h"

class Orbit;
class Rotation;
class Frame;

class OFSAPI StateVectors
{
public:
    // All state vectors in assciated frame
    glm::dvec3 pos;        // position
    glm::dvec3 vel;        // linear velocity
    
    glm::dmat4 R;          // rotation matrix
    glm::dquat Q;          // orientation
    glm::dvec3 omega;      // angular velocity
};

class OFSAPI Object
{
public:
    // enum oldObjectType
    // {
    //     objUnknown = 0,
    //     objPlayer,
    //     objVessel,
    //     objCelestialStar,
    //     objCelestialBody,
    // };

    Object(const cstr_t &name, ObjectType type)
    : objType(type)
    {
        objNames[0] = name;
    }
    
    virtual ~Object() = default;

    inline void setsName(str_t name)            { objNames[0] = name; }    
    inline str_t getName() const                { return objNames[0]; }
    inline str_t getsName() const               { return objNames[0]; }
    inline ObjectType getType() const           { return objType; }
    inline double getRadius() const             { return radius; }
    inline double getBoundingRadius() const     { return radius * std::numbers::sqrt3; }
    inline double getCullingRadius() const      { return cullingRadius; }   
    inline glm::dvec3 getSemiAxes() const       { return semiAxes; }
    inline double getMass() const               { return mass; }

    inline bool isSphere() const                { return semiAxes.x == semiAxes.y && semiAxes.x == semiAxes.z; }

    inline void setMass(double val)             { mass = val; }
    inline void setAlbedo(double val)           { albedo = val; }
    inline void setRadius(double val)           { radius = val; semiAxes = { val, val, val }; }
    inline void setSemiAxes(glm::dvec3 axes)    { semiAxes = axes; }

    // Virtual function calls
    virtual void setOrbitFrame(Frame *frame) = 0;
    virtual void setBodyFrame(Frame *frame) = 0;
    virtual void setOrbit(Orbit *nOrbit) = 0;
    virtual void setRotation(Rotation *nRotation) = 0;
    virtual void updateCullingRadius();
    virtual void update(bool force);

    virtual Frame *getOrbitFrame() const = 0;
    virtual Frame *getBodyFrame() const = 0;
    virtual Orbit *getOrbit() const = 0;
    virtual Rotation *getRotation() const = 0;

    virtual glm::dvec3 getuPosition(double tjd) const;
    virtual glm::dvec3 getuVelocity(double tjd) const;
    virtual glm::dmat3 getuOrientation(double tjd) const;

    virtual glm::dvec3 getoPosition(double tjd) const;
    virtual glm::dvec3 getoVelocity(double tjd) const;

private:
    ObjectType objType = objUnknown;
    std::vector<str_t> objNames{1};

public:
    StateVectors s0;    // current state vectors at time t0
    StateVectors s1;    // new state vectors at time t0+dt during update function call

protected:
    glm::dvec3 objPosition = { 0, 0, 0 };
    glm::dvec3 objVelocity = { 0, 0, 0 };
    glm::dmat3 objRotation = glm::dmat3(1.0);

    glm::dvec3 cg = { 0, 0, 0 };    // center gravity for position/rotation

    double  mass     = 0.0;
    double  radius   = 0.0;
    double  albedo   = 0.0;
    glm::dvec3 semiAxes = { 0, 0, 0 };
    double  cullingRadius = 0.0;

    glm::dvec3 rposBase, rposAdd;  // base/incremental of relative position
    glm::dvec3 rvelBase, rvelAdd;  // baae/incremental of relative velocity
    glm::dmat3 rrotBase, rrotAdd;  // base/incremental of relative orientation

    Object *cbody = nullptr;    // orbit reference body

    std::vector<ReferenceMark *> referenceMarks;
};