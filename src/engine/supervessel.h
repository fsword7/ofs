// supervessel.h - super vessel package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#pragma once

#include "engine/rigidbody.h"

struct VesselList
{
    Vessel *vessel;     // Attached/docked vessel
    glm::dvec3 rpos;    // relative position in supervessel coords
    glm::dmat3 rrot;    // relative orientation (rotation matrix)
    glm::dquat rq;      // relative orientation (quaternion)
};

class SuperVessel : public RigidBody
{
public:
    SuperVessel();
    virtual ~SuperVessel() = default;

    bool addVessel(Vessel *vessel1, int port1, Vessel *vessel2, int port2);
    void attach(Vessel *vessel);
    void detach(Vessel *vessel);
    
    void getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double dt);

    void updateMassCG();

    void update(bool force);

private:
    std::vector<Vessel *> vessels;
    std::vector<VesselList> vlist;
};