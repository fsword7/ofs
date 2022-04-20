// orbit.cpp - Orbit package
//
// Author:  Tim Stark
// Date:    Mar 7, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/orbit.h"

vec3d_t CachingOrbit::getPosition(double jd) const
{
    if (jd != lastTime)
    {
        lastPosition  = calculatePosition(jd);
        lastTime      = jd;
        positionValid = true;
        velocityValid = false;
    }
    else if (positionValid == false)
    {
        lastPosition  = calculatePosition(jd);
        positionValid = true;
    }

    return lastPosition;
}

vec3d_t CachingOrbit::getVelocity(double jd) const
{
    if (jd != lastTime)
    {
        lastVelocity  = calculateVelocity(jd);
        lastTime      = jd;
        positionValid = false;
        velocityValid = true;
    }
    else if (positionValid == false)
    {
        lastVelocity  = calculateVelocity(jd);
        velocityValid = true;
    }

    return lastVelocity;
}

vec3d_t CachingOrbit::calculateVelocity(double jd) const
{
    return vec3d_t(0, 0, 0);
}