// rotation.h - Rotation Model package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#pragma once

class Rotation
{
public:
    virtual ~Rotation() = default;

    glm::dmat3 getRotation(double tjd) const;

    virtual glm::dmat3 spin(double tjd) const = 0;
    virtual glm::dvec3 getAngularVelocity(double tjd) const;
    virtual glm::dmat3 getEquatorRotation(double) const;

    virtual double getPeriod() const { return 0.0; }
    virtual bool isPeriodic() const  { return false; }

    static Rotation *create(cstr_t &name);
};

class UniformRotation : public Rotation
{
public:
    UniformRotation(double epoch, double offset,
        double inclination, double ascendingNode,
        double period)
    : epoch(epoch), offset(offset), inclination(inclination),
        ascendingNode(ascendingNode), period(period)
    { }

    glm::dmat3 spin(double tjd) const;
    glm::dmat3 getEquatorOrientation(double tjd) const;
    glm::dvec3 getAngularVelocity(double tjd) const;

    virtual double getPeriod() const override  { return period; }
    virtual bool   isPeriodic() const override { return true; }

protected:
    double epoch = 0.0;
    double offset = 0.0;
    double inclination = 0.0;
    double ascendingNode = 0.0;
    double period = 0.0;
};

class CachingRotation : public Rotation
{
public:
    CachingRotation() = default;
    virtual ~CachingRotation() = default;

    glm::dmat3 spin(double tjd) const;
    glm::dmat3 getEquatorRotation(double tjd) const;
    glm::dvec3 getAngularVelocity(double tjd) const;

    virtual glm::dmat3 computeSpin(double tjd) const = 0;
    virtual glm::dmat3 computeEquatorRotation(double tjd) const = 0;
    virtual glm::dvec3 computeAngularVelocity(double tjd) const;
    virtual double  getPeriod() const = 0;
    virtual bool    isPeriodic() const = 0;

private:
    mutable glm::dmat3 lastSpin = glm::dmat3(1.0);
    mutable glm::dmat3 lastEquator = glm::dmat3(1.0);
    mutable glm::dvec3 lastVelocity = { 0, 0, 0 };

    mutable double lastTime = 0;
    mutable bool   validSpin = false;
    mutable bool   validEquator = false;
    mutable bool   validVelocity = false;
};

class EarthRotation : public CachingRotation
{
public:
    EarthRotation() = default;

    glm::dmat3 computeSpin(double tjd) const override;
    glm::dmat3 computeEquatorRotation(double tjd) const override;

    double getPeriod() const override   { return 23.9344694 / 24.0; }
    bool isPeriodic() const override    { return true; }
};

class IAURotation : public CachingRotation
{
public:
    IAURotation(double period) : period(period)
    { }
    ~IAURotation() = default;

    virtual glm::dmat3 computeSpin(double tjd) const override;
    virtual glm::dmat3 computeEquatorRotation(double tjd) const override;
    // virtual glm::dvec3 computeAngularVelocity(double tjd) const override;

    virtual double getPeriod() const override { return period; }
    virtual bool isPeriodic() const override  { return true; }

    virtual glm::dvec2 computePole(double tjd) const = 0;
    virtual double computeMeridian(double tjd) const = 0;

protected:
    double period = 0.0;
    bool   reversal = false;
};

class IAUPrecissionRotation : public IAURotation
{
public:
    IAUPrecissionRotation(double poleRA, double poleRARate,
        double poleDec, double poleDecRate,
        double meridian, double rate)
    : IAURotation(abs(360.0 / rate)),
        poleRA(poleRA), poleRARate(poleRARate),
        poleDec(poleDec), poleDecRate(poleDecRate),
        meridianAtEpoch(meridian), rotationRate(rate)
    {
        if (rate < 0.0)
            reversal = true;
    }

    glm::dvec2 computePole(double tjd) const override;
    double computeMeridian(double tjd) const override;

private:
    double poleRA, poleRARate;
    double poleDec, poleDecRate;
    double meridianAtEpoch;
    double rotationRate;
};
