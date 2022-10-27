// orbit.cpp - Orbit package
//
// Author:  Tim Stark
// Date:    Mar 7, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/orbit.h"

glm::dvec3 CachingOrbit::getPosition(double jd) const
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

glm::dvec3 CachingOrbit::getVelocity(double jd) const
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

glm::dvec3 CachingOrbit::calculateVelocity(double jd) const
{
    return { 0, 0, 0 };
}