// supervessel.h - super vessel package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#pragma once

#include "engine/rigidbody.h"

class SuperVessel : public RigidBody
{
public:
    SuperVessel();
    virtual ~SuperVessel() = default;

    bool addVessel(Vessel *vessel);
    void attach(Vessel *vessel);
    void detach(Vessel *vessel);
    
    void getIntermediateMoments(vec3d_t &acc, vec3d_t &am, const StateVectors &state, double dt);

    void update();

private:
    std::vector<Vessel *> vessels;
};