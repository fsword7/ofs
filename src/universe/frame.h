// frame.h - Frame Reference package
//
// Author:  Tim Stark
// Date:    Apt 21, 2022

#pragma once

#include "universe/astro.h"

class Object;
class Frame;
class celBody;
class celStar;

class FrameTree
{
public:
    FrameTree(celBody *body);
    FrameTree(celStar *star);
    virtual ~FrameTree();

    inline celStar *getStar() const         { return parentStar; }
    inline celBody *getBody() const         { return parentBody; }
    inline Frame *getDefaultFrame() const   { return defaultFrame; }
    inline int getSystemSize() const        { return objects.size(); }
    inline bool isRoot() const              { return parentBody == nullptr; }
    
    void addObject(Object *object);
    Object *getObject(int idx) const;

private:
    celStar *parentStar = nullptr;
    celBody *parentBody = nullptr;

    std::vector<Object *> objects;

    Frame *defaultFrame = nullptr;
};

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

    virtual quatd_t getOrientation(double tjd) const = 0;

    vec3d_t fromUniversal(const vec3d_t &upos, double tjd);
    quatd_t fromUniversal(const quatd_t &urot, double tjd);

    vec3d_t toUniversal(const vec3d_t &lpos, double tjd);
    quatd_t toUniversal(const quatd_t &lrot, double tjd);

    static Frame *create(cstr_t &frameName, Object *bodyObject, Object *parentObject);

private:
    Frame *parentFrame = nullptr;
    mutable int refCount = 0;

protected:
    Object *center = nullptr;
    str_t frameName;
};


// class CachingFrame : public Frame
// {
// public:
//     CachingFrame() = default;
//     virtual ~CachingFrame() = default;
// };

class J2000EclipticFrame : public Frame
{
public:
    J2000EclipticFrame(Object *object, Frame *parent = nullptr);
    ~J2000EclipticFrame() = default;

    quatd_t getOrientation(double) const override
    {
        return quatd_t::Identity();
    }
};

class J2000EquatorFrame : public Frame
{
public:
    J2000EquatorFrame(Object *object, Frame *parent = nullptr);
    ~J2000EquatorFrame() = default;

    quatd_t getOrientation(double) const override
    {
        // return quatd_t(vec3d_t(J2000Obliquity, 0, 0));
        return quatd_t(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX()));
    }
};

class BodyFixedFrame : public Frame
{
public:
    BodyFixedFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~BodyFixedFrame() = default;

    quatd_t getOrientation(double) const override;

private:
    Object *fixedObject = nullptr;
};

class BodyMeanEquatorFrame : public Frame
{
public:
    BodyMeanEquatorFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~BodyMeanEquatorFrame() = default;

    quatd_t getOrientation(double) const override;

private:
    Object *equatorObject = nullptr;
};

class ObjectSyncFrame : public Frame
{
public:
    ObjectSyncFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~ObjectSyncFrame() = default;

    quatd_t getOrientation(double) const override;

private:
    Object *targetObject = nullptr;
};
