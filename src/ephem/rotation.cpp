// rotation.cpp - Rotation Model package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "universe/astro.h"
#include "ephem/rotation.h"
// #include "ephem/precession.h"

// ******** Rotation Model ********

quatd_t Rotation::getEquatorRotation(double tjd) const
{
    return quatd_t(1, 0, 0, 0);
}

vec3d_t Rotation::getAngularVelocity(double tjd) const
{
    return vec3d_t(0, 0, 0);
}

quatd_t Rotation::getRotation(double tjd) const
{
    return spin(tjd) * getEquatorRotation(tjd);
}

// ******** Uniform Rotational Model ********

quatd_t UniformRotation::spin(double tjd) const
{
    double offsetSpin = (tjd - epoch) / period;
    double spin = offsetSpin - floor(offsetSpin);

    return yRotate((-spin * pi * 2.0) - offset);
}

quatd_t UniformRotation::getEquatorOrientation(double tjd) const
{
    return xRotate(-inclination) * yRotate(-ascendingNode);
}

vec3d_t UniformRotation::getAngularVelocity(double tjd) const
{
    return vec3d_t(0, 0, 0);
}

// ******** Caching Rotation Model ********

quatd_t CachingRotation::spin(double tjd) const
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

quatd_t CachingRotation::getEquatorRotation(double tjd) const
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

vec3d_t CachingRotation::getAngularVelocity(double tjd) const
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

vec3d_t CachingRotation::computeAngularVelocity(double tjd) const
{
    return vec3d_t( 0, 0, 0 );
}
