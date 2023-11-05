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
    
    glm::dmat3 R;          // rotation matrix
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

    Object(json &cfg, ObjectType type);
    
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
    inline double getAlbedo() const             { return geomAlbedo; }
    inline color_t getColor() const             { return geomColor; }

    inline bool isSphere() const                { return semiAxes.x == semiAxes.y && semiAxes.x == semiAxes.z; }

    inline void setMass(double val)             { mass = val; }
    inline void setAlbedo(double val)           { geomAlbedo = val; }
    inline void setColor(color_t color)         { geomColor = color; }
    inline void setRadius(double val)           { radius = val; semiAxes = { val, val, val }; }
    inline void setSemiAxes(glm::dvec3 axes)    { semiAxes = axes; }

    // Virtual function calls
    virtual void setOrbitFrame(Frame *frame) = 0;
    virtual void setBodyFrame(Frame *frame) = 0;
    virtual void setOrbit(Orbit *nOrbit) = 0;
    virtual void setRotation(Rotation *nRotation) = 0;
    virtual void updateCullingRadius();
    virtual void update(bool force);

    virtual void beginUpdate();
    virtual void endUpdate();

    virtual Frame *getOrbitFrame() const = 0;
    virtual Frame *getBodyFrame() const = 0;
    virtual Orbit *getOrbit() const = 0;
    virtual Rotation *getRotation() const = 0;

    virtual glm::dvec3 getuPosition(double tjd) const;
    virtual glm::dvec3 getuVelocity(double tjd) const;
    virtual glm::dmat3 getuOrientation(double tjd) const;
    virtual glm::dquat getqOrientation(double tjd) const;

    virtual glm::dvec3 getoPosition(double tjd) const;
    virtual glm::dvec3 getoVelocity(double tjd) const;

    virtual glm::dvec3 getoPosition() const     { return objPosition; }
    virtual glm::dvec3 getoVelocity() const     { return objVelocity; }
    virtual glm::dvec3 getbPosition() const     { return baryPosition; }
    virtual glm::dvec3 getbVelocity() const     { return baryVelocity; }

    virtual double getLuminosity(double lum, double dist) const { return 0; }
    virtual double getApparentMagnitude(glm::dvec3 sun, double irradiance, glm::dvec3 view) const { return 0; }

    void getValueReal(json &data, cstr_t &name, double &value);
    void getValueString(json &data, cstr_t &name, str_t &value);

private:
    ObjectType objType = objUnknown;
    std::vector<str_t> objNames{1};

public:
    // StateVectors sv[2];
    // StateVectors &s0 = sv[0];   // current state vectors at time t0
    // StateVectors &s1 = sv[1];   // new state vectors at time t0+dt during update function call
    StateVectors s0;    // current state vectors at time t0
    StateVectors s1;    // new state vectors at time t0+dt during update function call

protected:
    glm::dvec3 objPosition = { 0, 0, 0 };
    glm::dvec3 objVelocity = { 0, 0, 0 };
    glm::dmat3 objRotation = glm::dmat3(1.0);
    glm::dquat objqRotation = glm::dquat();

    glm::dvec3 baryPosition = { 0, 0, 0 };
    glm::dvec3 baryVelocity = { 0, 0, 0 };

    glm::dvec3 cg = { 0, 0, 0 };    // center gravity for position/rotation

    double  mass        = 0.0;
    double  radius      = 0.0;
    double  geomAlbedo  = 0.0;
    color_t geomColor   = { 1.0, 1.0, 1.0, 1.0 };

    glm::dvec3 semiAxes = { 0, 0, 0 };
    double  cullingRadius = 0.0;

    glm::dvec3 rposBase, rposAdd;  // base/incremental of relative position
    glm::dvec3 rvelBase, rvelAdd;  // baae/incremental of relative velocity
    glm::dmat3 rrotBase, rrotAdd;  // base/incremental of relative orientation

    Object *cbody = nullptr;    // orbit reference body

    std::vector<ReferenceMark *> referenceMarks;
};