// frame.cpp - Frame Reference package
//
// Author:  Tim Stark
// Date:    Apt 21, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/frame.h"

// ******** Reference Frame Tree ********

FrameTree::FrameTree(celStar *star)
: parentStar(star)
{
    defaultFrame = new J2000EclipticFrame(star);
}

FrameTree::FrameTree(celBody *body)
: parentBody(body)
{
    defaultFrame = new BodyMeanEquatorFrame(body, body);
}

FrameTree::~FrameTree()
{
    if (defaultFrame != nullptr)
        defaultFrame->release();
}

void FrameTree::addObject(Object *object)
{
    objects.push_back(object);
}

Object *FrameTree::getObject(int idx) const
{
    if (idx >= 0 && idx < objects.size())
        return objects[idx];
    return nullptr;
}

// ******** Reference Frame ********

int Frame::lock() const
{
    return ++refCount;
}

int Frame::release() const
{
    int count = --refCount;
    if (refCount < 0)
        delete this;
    return count;
}


vec3d_t Frame::fromUniversal(const vec3d_t &upos, double tjd)
{
    if (center == nullptr)
        return upos;
    
    vec3d_t cpos = center->getuPosition(tjd);
    quatd_t crot = getOrientation(tjd);
    vec3d_t rpos = crot * (upos - cpos);

    return rpos;
}

quatd_t Frame::fromUniversal(const quatd_t &urot, double tjd)
{
    if (center == nullptr)
        return urot;
    return urot * getOrientation(tjd).conjugate();
}

vec3d_t Frame::toUniversal(const vec3d_t &lpos, double tjd)
{
    if (center == nullptr)
        return lpos;

    vec3d_t cpos = center->getuPosition(tjd);
    quatd_t crot = getOrientation(tjd);
    vec3d_t rpos = cpos + (crot.conjugate() * lpos);

    return rpos;
}

quatd_t Frame::toUniversal(const quatd_t &lrot, double tjd)
{
    if (center == nullptr)
        return lrot;
    return lrot * getOrientation(tjd);
}


Frame *Frame::create(cstr_t &frameName, Object *bodyObject, Object *parentObject)
{
    if (frameName == "EquatorJ2000")
        return new J2000EquatorFrame(bodyObject);
    if (frameName == "EclipticJ2000")
        return new J2000EclipticFrame(bodyObject);
    return nullptr;
}

// ******** J2000 Earth Ecliptic Reference Frame ********

J2000EclipticFrame::J2000EclipticFrame(Object *object, Frame *parent)
: Frame(object, parent)
{
    frameName = "J2000 Earth Ecliptic Reference Frame";
}

// ******** J2000 Earth Equator Reference Frame ********

J2000EquatorFrame::J2000EquatorFrame(Object *object, Frame *parent)
: Frame(object, parent)
{
    frameName = "J2000 Earth Equator Reference Frame";
}

// ******** Body Fixed Reference Frame ********

BodyFixedFrame::BodyFixedFrame(Object *object, Object *target, Frame *parent)
: Frame(object, parent), fixedObject(target)
{
    frameName = "Body Fixed Reference Frame";
}

quatd_t BodyFixedFrame::getOrientation(double tjd) const
{
    quatd_t yrot180 = { 0, 0, 1, 0 };

    switch (fixedObject->getType())
    {
    case Object::objCelestialStar:
    case Object::objCelestialBody:
        return yrot180 * dynamic_cast<celBody *>(fixedObject)->getuOrientation(tjd);
    default:
        return yrot180; // { 1, 0, 0, 0 };
    }
}

// ******** Body Mean Equator Reference Frame ********

BodyMeanEquatorFrame::BodyMeanEquatorFrame(Object *object, Object *target, Frame *parent)
: Frame(object, parent), equatorObject(target)
{
    frameName = "Body Mean Equator Reference Frame";
}

quatd_t BodyMeanEquatorFrame::getOrientation(double tjd) const
{
    switch (equatorObject->getType())
    {
    case Object::objCelestialStar:
    case Object::objCelestialBody:
        return dynamic_cast<celBody *>(equatorObject)->getEquatorial(tjd);
    default:
        return quatd_t::Identity();
    }
}

// ******** Object Sync Reference Frame ********

ObjectSyncFrame::ObjectSyncFrame(Object *object, Object *target, Frame *parent)
: Frame(object, parent), targetObject(target)
{
    frameName = "Object Sync Reference Frame";
}

quatd_t ObjectSyncFrame::getOrientation(double tjd) const
{
    vec3d_t opos = center->getuPosition(tjd);
    vec3d_t tpos = targetObject->getuPosition(tjd);

    return ofs::lookAt(opos, tpos, vec3d_t(0, 1, 0));
}
