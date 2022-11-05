// vbody.cpp - Visual Celestial Body package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "client.h"
#include "vobject.h"
#include "vbody.h"
#include "surface.h"

vBody::vBody(ObjectHandle object, Scene &scene)
: vObject(object, scene)
{
    smgr = new SurfaceManager(object, scene);
}

vBody::~vBody()
{
    
}

void vBody::update(int now)
{
    vObject::update(now);

}

void vBody::render(const ObjectProperties &op)
{
    smgr->render(dmWorld, op);
}