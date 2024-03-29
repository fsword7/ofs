// frame.h - Frame Reference package
//
// Author:  Tim Stark
// Date:    Apt 21, 2022

#pragma once

#include "universe/astro.h"

class Object;
class Frame;
class CelestialBody;
class CelestialStar;

class FrameTree
{
public:
    FrameTree(CelestialBody *body);
    FrameTree(CelestialStar *star);
    virtual ~FrameTree();

    inline CelestialStar *getStar() const   { return parentStar; }
    inline CelestialBody *getBody() const   { return parentBody; }
    inline Frame *getDefaultFrame() const   { return defaultFrame; }
    inline int getSystemSize() const        { return objects.size(); }
    inline bool isRoot() const              { return parentBody == nullptr; }
    
    void addObject(Object *object);
    // Object *getObject(int idx) const;

    Object *getObject(int idx) const
    {
        if (idx >= 0 && idx < objects.size())
            return objects[idx];
        return nullptr;
    }


private:
    CelestialStar *parentStar = nullptr;
    CelestialBody *parentBody = nullptr;

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

    virtual glm::dmat3 getOrientation(double tjd) const = 0;

    glm::dvec3 fromUniversal(const glm::dvec3 &upos, double tjd);
    glm::dmat3 fromUniversal(const glm::dmat3 &urot, double tjd);

    glm::dvec3 toUniversal(const glm::dvec3 &lpos, double tjd);
    glm::dmat3 toUniversal(const glm::dmat3 &lrot, double tjd);

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

    glm::dmat3 getOrientation(double) const override
    {
        return glm::dmat3(1.0);
    }
};

class J2000EquatorFrame : public Frame
{
public:
    J2000EquatorFrame(Object *object, Frame *parent = nullptr);
    ~J2000EquatorFrame() = default;

    glm::dmat3 getOrientation(double) const override
    {
        // return quatd_t(vec3d_t(J2000Obliquity, 0, 0));
        // return quatd_t(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX()));
        return glm::dmat3(1.0);
    }
};

class BodyFixedFrame : public Frame
{
public:
    BodyFixedFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~BodyFixedFrame() = default;

    glm::dmat3 getOrientation(double) const override;

private:
    Object *fixedObject = nullptr;
};

class BodyMeanEquatorFrame : public Frame
{
public:
    BodyMeanEquatorFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~BodyMeanEquatorFrame() = default;

    glm::dmat3 getOrientation(double) const override;

private:
    Object *equatorObject = nullptr;
};

class ObjectSyncFrame : public Frame
{
public:
    ObjectSyncFrame(Object *object, Object *target, Frame *parent = nullptr);
    ~ObjectSyncFrame() = default;

    glm::dmat3 getOrientation(double) const override;

private:
    Object *targetObject = nullptr;
};
