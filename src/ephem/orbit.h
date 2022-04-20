// orbit.h - Orbit package
//
// Author:  Tim Stark
// Date:    Apr 20, 2022

#pragma once

class Orbit
{
public:
    Orbit() = default;
    virtual ~Orbit() = default;

    virtual vec3d_t getPosition(double tjd) const = 0;
    virtual vec3d_t getVelocity(double tjd) const = 0;

    virtual bool   isPeriodic() const { return true; }
    virtual double getPeriod() const = 0;
    virtual double getBoundingRadius() const = 0;
};

class CachingOrbit : public Orbit
{
public:
    CachingOrbit() = default;
    virtual ~CachingOrbit() = default;

    vec3d_t getPosition(double tjd) const;
    vec3d_t getVelocity(double tjd) const;

    virtual vec3d_t calculatePosition(double jd) const = 0;
    virtual vec3d_t calculateVelocity(double jd) const;

private:
    // Must use 'mutable' to allow variables modifiable in const objects
    mutable double lastTime = -std::numeric_limits<double>::infinity();
    mutable vec3d_t lastPosition = vec3d_t(0, 0, 0);
    mutable vec3d_t lastVelocity = vec3d_t(0, 0, 0);

    mutable bool positionValid = false;
    mutable bool velocityValid = false;
};