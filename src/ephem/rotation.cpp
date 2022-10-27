// rotation.cpp - Rotation Model package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/rotation.h"
// #include "ephem/precession.h"

// ******** Rotation Model ********

glm::dmat3 Rotation::getEquatorRotation(double tjd) const
{
    return glm::dmat3(1.0);
}

glm::dvec3 Rotation::getAngularVelocity(double tjd) const
{
    return { 0, 0, 0 };
}

glm::dmat3 Rotation::getRotation(double tjd) const
{
    return spin(tjd) * getEquatorRotation(tjd);
}

// ******** Uniform Rotational Model ********

glm::dmat3 UniformRotation::spin(double tjd) const
{
    double offsetSpin = (tjd - epoch) / period;
    double spin = offsetSpin - floor(offsetSpin);

    return yRotate((-spin * pi * 2.0) - offset);
}

glm::dmat3 UniformRotation::getEquatorOrientation(double tjd) const
{
    return xRotate(-inclination) * yRotate(-ascendingNode);
}

glm::dvec3 UniformRotation::getAngularVelocity(double tjd) const
{
    return { 0, 0, 0 };
}

// ******** Caching Rotation Model ********

glm::dmat3 CachingRotation::spin(double tjd) const
{
    if (tjd != lastTime)
    {
        lastSpin        = computeSpin(tjd);
        lastTime        = tjd;
        validSpin       = true;
        validEquator    = false;
        validVelocity   = false;
    }
    else if (validSpin == false)
    {
        lastSpin        = computeSpin(tjd);
        validSpin       = true;
    }

    return lastSpin;
}

glm::dmat3 CachingRotation::getEquatorRotation(double tjd) const
{
    if (tjd != lastTime)
    {
        lastEquator     = computeEquatorRotation(tjd);
        lastTime        = tjd;
        validSpin       = false;
        validEquator    = true;
        validVelocity   = false;
    }
    else if (validSpin == false)
    {
        lastEquator     = computeEquatorRotation(tjd);
        validEquator    = true;
    }


    return lastEquator;
}

glm::dvec3 CachingRotation::getAngularVelocity(double tjd) const
{
    if (tjd != lastTime)
    {
        lastVelocity    = computeAngularVelocity(tjd);
        lastTime        = tjd;
        validSpin       = false;
        validEquator    = false;
        validVelocity   = true;
    }
    else if (validSpin == false)
    {
        lastVelocity    = computeAngularVelocity(tjd);
        validVelocity   = true;
    }

    return lastVelocity;
}

glm::dvec3 CachingRotation::computeAngularVelocity(double tjd) const
{
    return { 0, 0, 0 };
}
