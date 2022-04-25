// rigidbody.h - rigid body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#pragma once

#include "engine/object.h"

class Frame;
class OrbitalElements;

class RigidBody : public Object
{
public:
    RigidBody(cstr_t &name, ObjectType type)
    : Object(name, type)
    { }
    
    virtual ~RigidBody() = default;

    void setOrbitFrame(Frame *frame) override       { orbitFrame = frame; }
    void setBodyFrame(Frame *frame) override        { bodyFrame = frame; }
    void setOrbit(Orbit *nOrbit) override           { orbit = nOrbit; }
    void setRotation(Rotation *nRotation)           { rotation = nRotation; }

    Frame *getOrbitFrame() const override   { return orbitFrame; }
    Frame *getBodyFrame() const override    { return bodyFrame; }
    Orbit *getOrbit() const override        { return orbit; }
    Rotation *getRotation() const override  { return rotation; }

    // orbtial positions
    vec3d_t getuPosition(double tjd) const override;
    quatd_t getuOrientation(double tjd) const override;
    vec3d_t getoPosition(double tjd) const override;

    void update();
    
protected:
    // Reference frame parameters
    Frame *orbitFrame = nullptr;
    Frame *bodyFrame = nullptr;

    Orbit *orbit = nullptr;
    Rotation *rotation = nullptr;

    OrbitalElements *elements = nullptr;
    bool orbitValid = false;

    vec3d_t cpos, cvel; // state vectors in reference frame
};