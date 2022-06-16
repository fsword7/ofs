// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Orbit;
class Rotation;
class Frame;

class StateVectors
{
public:
    // All state vectors in assciated frame
    vec3d_t pos;        // position
    vec3d_t vel;        // linear velocity
    
    mat4d_t R;          // rotation matrix
    quatd_t Q;          // orientation
    vec3d_t omega;      // angular velocity
};

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

    inline void setsName(str_t name)        { objNames[0] = name; }    
    inline str_t getName() const            { return objNames[0]; }
    inline str_t getsName() const           { return objNames[0]; }
    inline ObjectType getType() const       { return objType; }
    inline double getRadius() const         { return radius; }
    inline double getBoundingRadius() const { return radius * std::numbers::sqrt3; }
    inline double getCullingRadius() const  { return cullingRadius; }   
    inline vec3d_t getSemiAxes() const      { return semiAxes; }

    inline bool isSphere() const            { return semiAxes.x() == semiAxes.y() && semiAxes.x() == semiAxes.z(); }

    inline void setMass(double val)         { mass = val; }
    inline void setAlbedo(double val)       { albedo = val; }
    inline void setRadius(double val)       { radius = val; semiAxes = { val, val, val }; }
    inline void setSemiAxes(vec3d_t axes)   { semiAxes = axes; radius = semiAxes.maxCoeff(); }

    // Virtual function calls
    virtual void setOrbitFrame(Frame *frame) = 0;
    virtual void setBodyFrame(Frame *frame) = 0;
    virtual void setOrbit(Orbit *nOrbit) = 0;
    virtual void setRotation(Rotation *nRotation) = 0;
    virtual void updateCullingRadius();

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

public:
    StateVectors s0;    // current state vectors at time t0
    StateVectors s1;    // new state vectors at time t0+dt during update function call

protected:
    vec3d_t objPosition = vec3d_t::Zero();
    vec3d_t objVelocity = vec3d_t::Zero();
    quatd_t objRotation = quatd_t::Identity();


    double  mass     = 0.0;
    double  radius   = 0.0;
    double  albedo   = 0.0;
    vec3d_t semiAxes = vec3d_t::Zero();
    double  cullingRadius = 0.0;

    vec3d_t rposBase, rposAdd;  // base/incremental of relative position
    vec3d_t rvelBase, rvelAdd;  // baae/incremental of relative velocity
    quatd_t rrotBase, rrotAdd;  // base/incremental of relative orientation

    Object *cbody = nullptr;    // orbit reference body

};