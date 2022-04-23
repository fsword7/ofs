// rotation.h - Rotation Model package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#pragma once

class Rotation
{
public:
    virtual ~Rotation() = default;

    quatd_t getRotation(double tjd) const;

    virtual quatd_t spin(double tjd) const = 0;
    virtual vec3d_t getAngularVelocity(double tjd) const;
    virtual quatd_t getEquatorRotation(double) const;

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

    quatd_t spin(double tjd) const;
    quatd_t getEquatorOrientation(double tjd) const;
    vec3d_t getAngularVelocity(double tjd) const;

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

    quatd_t spin(double tjd) const;
    quatd_t getEquatorRotation(double tjd) const;
    vec3d_t getAngularVelocity(double tjd) const;

    virtual quatd_t computeSpin(double tjd) const = 0;
    virtual quatd_t computeEquatorRotation(double tjd) const = 0;
    virtual vec3d_t computeAngularVelocity(double tjd) const;
    virtual double  getPeriod() const = 0;
    virtual bool    isPeriodic() const = 0;

private:
    mutable quatd_t lastSpin = { 1, 0, 0, 0 };
    mutable quatd_t lastEquator = { 1, 0, 0, 0 };
    mutable vec3d_t lastVelocity = { 0, 0, 0 };

    mutable double lastTime = 0;
    mutable bool   validSpin = false;
    mutable bool   validEquator = false;
    mutable bool   validVelocity = false;
};

class EarthRotation : public CachingRotation
{
public:
    EarthRotation() = default;

    quatd_t computeSpin(double tjd) const override;
    quatd_t computeEquatorRotation(double tjd) const override;

    double getPeriod() const override   { return 23.9344694 / 24.0; }
    bool isPeriodic() const override    { return true; }
};

class IAURotation : public CachingRotation
{
public:
    IAURotation(double period) : period(period)
    { }
    ~IAURotation() = default;

    virtual quatd_t computeSpin(double tjd) const override;
    virtual quatd_t computeEquatorRotation(double tjd) const override;
    // virtual vec3d_t computeAngularVelocity(double tjd) const override;

    virtual double getPeriod() const override { return period; }
    virtual bool isPeriodic() const override  { return true; }

    virtual vec2d_t computePole(double tjd) const = 0;
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

    vec2d_t computePole(double tjd) const override;
    double computeMeridian(double tjd) const override;

private:
    double poleRA, poleRARate;
    double poleDec, poleDecRate;
    double meridianAtEpoch;
    double rotationRate;
};
