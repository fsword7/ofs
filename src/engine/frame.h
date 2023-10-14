// frame.h - Player Frame package for free-travel mode
//
// Author:  Tim Stark
// Date:    Oct 13, 2023

#pragma once

class Object;

class Frame
{
public:
    Frame(Object *object, Frame *parent = nullptr)
    : center(object), parentFrame(parent)
    {
        lock();
    }

    virtual ~Frame() = default;

    inline Object *getCenter() const        { return center; }
    inline cstr_t getsName() const          { return frameName; }
    inline Frame *getParentFrame() const    { return parentFrame; }
    inline bool isRoot() const              { return parentFrame == nullptr; }

    int lock() const;
    int release() const;

    virtual glm::dquat getOrientation(double tjd) const = 0;
    // virtual glm::dmat3 getOrientation(double tjd) const = 0;

    glm::dvec3 fromUniversal(const glm::dvec3 &upos, double tjd);
    glm::dquat fromUniversal(const glm::dquat &urot, double tjd);
    // glm::dmat3 fromUniversal(const glm::dmat3 &urot, double tjd);

    glm::dvec3 toUniversal(const glm::dvec3 &lpos, double tjd);
    glm::dquat toUniversal(const glm::dquat &lrot, double tjd);
    // glm::dmat3 toUniversal(const glm::dmat3 &lrot, double tjd);

    static Frame *create(cstr_t &frameName, Object *bodyObject, Object *parentObject);

private:
    Frame *parentFrame = nullptr;
    mutable int refCount = 0;

protected:
    Object *center = nullptr;
    str_t frameName;
};

class PlayerFrame
{
public:
    enum coordType
    {
        csUniversal  = 0,
        csEcliptical = 1,
        csEquatorial = 2,
        csBodyFixed  = 3,
        csBodyMeanEquator = 4,
        csObjectSync = 5
    };

    PlayerFrame();
    PlayerFrame(coordType csType, Object *center = nullptr, Object *targer = nullptr);
    ~PlayerFrame();

    static Frame *create(coordType csType, Object *center = nullptr, Object *targer = nullptr);

    coordType getType() const   { return type; }
    Frame *getFrame() const     { return frame; }

    str_t getsName() const
    { 
        return frame != nullptr ? frame->getCenter()->getsName() : "(Unknown)";
    }

    Object *getCenter() const
    {
        return frame != nullptr ? frame->getCenter() : nullptr;
    }

    glm::dvec3 fromUniversal(glm::dvec3 upos, double tjd);
    glm::dquat fromUniversal(glm::dquat urot, double tjd);
    glm::dvec3 toUniversal(glm::dvec3 lpos, double tjd);
    glm::dquat toUniversal(glm::dquat lrot, double tjd);
    
private:
    coordType type = csUniversal;
    Frame *frame = nullptr;
};

class J2000EclipticFrame : public Frame
{
public:
    J2000EclipticFrame(Object *object, Frame *parent = nullptr);
    ~J2000EclipticFrame() = default;

    glm::dquat getOrientation(double) const override
    {
        return glm::dquat();
    }

    // glm::dmat3 getOrientation(double) const override
    // {
    //     return glm::dmat3(1.0);
    // }
};

class J2000EquatorFrame : public Frame
{
public:
    J2000EquatorFrame(Object *object, Frame *parent = nullptr);
    ~J2000EquatorFrame() = default;

    glm::dquat getOrientation(double) const override
    {
        // return quatd_t(vec3d_t(J2000Obliquity, 0, 0));
        // return quatd_t(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX()));
        return glm::dquat();
    }

    // glm::dmat3 getOrientation(double) const override
    // {
    //     // return quatd_t(vec3d_t(J2000Obliquity, 0, 0));
    //     // return quatd_t(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX()));
    //     return glm::dmat3(1.0);
    // }
};

class BodyFixedFrame : public Frame
{
public:
    BodyFixedFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~BodyFixedFrame() = default;

    glm::dquat getOrientation(double) const override;
    // glm::dmat3 getOrientation(double) const override;

private:
    Object *fixedObject = nullptr;
};

class BodyMeanEquatorFrame : public Frame
{
public:
    BodyMeanEquatorFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~BodyMeanEquatorFrame() = default;

    glm::dquat getOrientation(double) const override;
    // glm::dmat3 getOrientation(double) const override;

private:
    Object *equatorObject = nullptr;
};

class ObjectSyncFrame : public Frame
{
public:
    ObjectSyncFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~ObjectSyncFrame() = default;

    glm::dquat getOrientation(double) const override;
    // glm::dmat3 getOrientation(double) const override;

private:
    Object *targetObject = nullptr;
};
