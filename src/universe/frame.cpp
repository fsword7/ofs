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

FrameTree::FrameTree(CelestialStar *star)
: parentStar(star)
{
    defaultFrame = new J2000EclipticFrame(star);
}

FrameTree::FrameTree(CelestialBody *body)
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

// Object *FrameTree::getObject(int idx) const
// {
//     if (idx >= 0 && idx < objects.size())
//         return objects[idx];
//     return nullptr;
// }

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


glm::dvec3 Frame::fromUniversal(const glm::dvec3 &upos, double tjd)
{
    if (center == nullptr)
        return upos;
    
    glm::dvec3 cpos = center->getuPosition(tjd);
    glm::dmat3 crot = getOrientation(tjd);
    glm::dvec3 rpos = crot * (upos - cpos);

    return rpos;
}

glm::dmat3 Frame::fromUniversal(const glm::dmat3 &urot, double tjd)
{
    if (center == nullptr)
        return urot;
    return urot * getOrientation(tjd); // .conjugate();
}

glm::dvec3 Frame::toUniversal(const glm::dvec3 &lpos, double tjd)
{
    if (center == nullptr)
        return lpos;

    glm::dvec3 cpos = center->getuPosition(tjd);
    glm::dmat3 crot = getOrientation(tjd);
    glm::dvec3 rpos = cpos + (crot /* .conjugate() */ * lpos);

    return rpos;
}

glm::dmat3 Frame::toUniversal(const glm::dmat3 &lrot, double tjd)
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

glm::dmat3 BodyFixedFrame::getOrientation(double tjd) const
{
    // glm::dquat yrot180 = { 0, 0, 1, 0 };
    glm::dmat3 yrot180 = glm::dmat3(1.0);
    
    switch (fixedObject->getType())
    {
    case ObjectType::objCelestialStar:
    case ObjectType::objCelestialBody:
        return yrot180 * dynamic_cast<CelestialBody *>(fixedObject)->getuOrientation(tjd);
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

glm::dmat3 BodyMeanEquatorFrame::getOrientation(double tjd) const
{
    switch (equatorObject->getType())
    {
    case ObjectType::objCelestialStar:
    case ObjectType::objCelestialBody:
        return dynamic_cast<CelestialBody *>(equatorObject)->getEquatorial(tjd);
    default:
        return glm::dmat3(1.0);
    }
}

// ******** Object Sync Reference Frame ********

ObjectSyncFrame::ObjectSyncFrame(Object *object, Object *target, Frame *parent)
: Frame(object, parent), targetObject(target)
{
    frameName = "Object Sync Reference Frame";
}

glm::dmat3 ObjectSyncFrame::getOrientation(double tjd) const
{
    glm::dvec3 opos = center->getuPosition(tjd);
    glm::dvec3 tpos = targetObject->getuPosition(tjd);

    return glm::lookAt(opos, tpos, { 0, 1, 0 });
}
