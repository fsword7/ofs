// vbody.cpp - Visual Celestial Body package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "engine/object.h"
#include "client.h"
#include "scene.h"
#include "vobject.h"
#include "vbody.h"
#include "surface.h"

vBody::vBody(const Object *object, Scene &scene)
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

void vBody::render(const ObjectListEntry &ole)
{
    smgr->renderBody(ole);
}