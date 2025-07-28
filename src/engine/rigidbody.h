// rigidbody.h - rigid body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#pragma once

#include "engine/celestial.h"
#include "ephem/elements.h"

class Frame;

class OFSAPI RigidBody : public Celestial
{
public:
    RigidBody(cstr_t &name, ObjectType type, celType cbtype)
    : Celestial(name, type, cbtype)
    { }

    RigidBody(cjson &config, ObjectType type, celType ctype);

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
    // glm::dvec3 getuPosition(double tjd) const override;
    // glm::dmat3 getuOrientation(double tjd) const override;
    // glm::dvec3 getoPosition(double tjd) const override;

    void updateGlobal(const glm::dvec3 &rpos, const::glm::dvec3 &rvel);

    virtual void update(bool force);

    virtual void getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt);
 
protected:
    // Reference frame parameters
    Frame *orbitFrame = nullptr;
    Frame *bodyFrame = nullptr;

    Orbit *orbit = nullptr;
    Rotation *rotation = nullptr;

    // OrbitalElements oel;
    // bool orbitValid = false;

    glm::dvec3 cpos, cvel;  // state vectors in reference frame
    glm::dvec3 acc;         // current linear accelration
    glm::dvec3 arot;        // current angular accelration
    glm::dvec3 pmi;         // principal moments of inertia
    glm::dvec3 torque;      // current torque of CG
};