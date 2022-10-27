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

    virtual glm::dvec3 getPosition(double tjd) const = 0;
    virtual glm::dvec3 getVelocity(double tjd) const = 0;

    virtual bool   isPeriodic() const { return true; }
    virtual double getPeriod() const = 0;
    virtual double getBoundingRadius() const = 0;
};

class CachingOrbit : public Orbit
{
public:
    CachingOrbit() = default;
    virtual ~CachingOrbit() = default;

    glm::dvec3 getPosition(double tjd) const override;
    glm::dvec3 getVelocity(double tjd) const override;
 
    virtual glm::dvec3 calculatePosition(double jd) const = 0;
    virtual glm::dvec3 calculateVelocity(double jd) const;

private:
    // Must use 'mutable' to allow variables modifiable in const objects
    mutable double lastTime = -std::numeric_limits<double>::infinity();
    mutable glm::dvec3 lastPosition = { 0, 0, 0 };
    mutable glm::dvec3 lastVelocity = { 0, 0, 0 };

    mutable bool positionValid = false;
    mutable bool velocityValid = false;
};