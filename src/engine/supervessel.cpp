// supervessel.cpp - super vessel package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "engine/vessel.h"
#include "engine/supervessel.h"

SuperVessel::SuperVessel()
: RigidBody("", objVessel)
{
    
}

bool SuperVessel::addVessel(Vessel *vessel)
{
    for (int idx = 0; idx < vessels.size(); idx++)
        if (vessels[idx] == vessel)
            return false;

    vessels.push_back(vessel);

    return true;
}

void SuperVessel::attach(Vessel *vessel)
{
    if (vessel->superVessel == nullptr)
        addVessel(vessel);
}

void SuperVessel::detach(Vessel *vessel)
{
    for (std::vector<Vessel *>::iterator p = vessels.begin(); p != vessels.end(); ++p)
        if (*p == vessel) {
            vessels.erase(p);
            break;
        }
}

void SuperVessel::getIntermediateMoments(vec3d_t &acc, vec3d_t &am, const StateVectors &state, double dt)
{

}

void SuperVessel::update()
{

}
