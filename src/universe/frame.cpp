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
    // defaultFrame = new BodyMeanEquatorFrame(body, body, nullptr);
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
    
    vec3d_t opos = center->getuPosition(tjd);
    quatd_t orot = glm::conjugate(getOrientation(tjd));
    vec3d_t rpos = (opos - upos) * orot;

    return rpos;
}

quatd_t Frame::fromUniversal(const quatd_t &urot, double tjd)
{
    if (center == nullptr)
        return urot;
    return urot * getOrientation(tjd);
}

vec3d_t Frame::toUniversal(const vec3d_t &lpos, double tjd)
{
    if (center == nullptr)
        return lpos;

    vec3d_t opos = center->getuPosition(tjd);
    quatd_t orot = getOrientation(tjd);
    vec3d_t rpos = opos + (lpos * orot);

    return rpos;
}

Frame *Frame::create(cstr_t &frameName, Object *bodyObject, Object *parentObject)
{
    if (frameName == "EquatorJ2000")
        return new J2000EquatorFrame(bodyObject, parentObject);
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

J2000EquatorFrame::J2000EquatorFrame(Object *object, Object *target, Frame *parent)
: Frame(object, parent)
{
    frameName = "J2000 Earth Equator Reference Frame";
}
