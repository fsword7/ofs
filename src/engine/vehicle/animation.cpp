// thruster.cpp - Vehicle package - animation
//
// Author:  Tim Stark
// Date:    Oct 23, 2024

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/vehicle/vehicle.h"

anim_t *Vehicle::createAnimation(int state)
{
    anim_t *an = new anim_t();

    an->dstate = state;
    an->state = state;
    an->compList.clear();

    animList.push_back(an);
    return an;
}

bool Vehicle::setAnimationState(int an, int state)
{
    if (an >= animList.size())
        return false;
    animList[an]->state = state;
    return true;
}

ancomp_t *Vehicle::addAnimationComponent(int an, double state0, double state1, ancomp_t *parent)
{
    if (an >= animList.size())
        return nullptr;

    ancomp_t *ac = new ancomp_t();
    ac->state0 = state0;
    ac->state1 = state1;
    ac->parent = parent;
    animList[an]->compList.push_back(ac);

    return ac;
}