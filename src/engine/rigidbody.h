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
    glm::dvec3 getuPosition(double tjd) const override;
    glm::dmat3 getuOrientation(double tjd) const override;
    glm::dvec3 getoPosition(double tjd) const override;

    virtual void update(bool force);
    
protected:
    // Reference frame parameters
    Frame *orbitFrame = nullptr;
    Frame *bodyFrame = nullptr;

    Orbit *orbit = nullptr;
    Rotation *rotation = nullptr;

    OrbitalElements *elements = nullptr;
    bool orbitValid = false;

    glm::dvec3 cpos, cvel; // state vectors in reference frame
};