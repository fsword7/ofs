// vstar.cpp - Visual Celestial Star package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
#include "engine/object.h"
#include "client.h"
#include "vobject.h"
#include "vstar.h"
#include "surface.h"

vStar::vStar(const Object *object, Scene &scene)
: vObject(object, scene)
{
    smgr = new SurfaceManager(object, scene);
}

vStar::~vStar()
{
    
}

void vStar::update(int now)
{
    vObject::update(now);

}

void vStar::render(const ObjectProperties &op)
{
    smgr->renderStar(dmWorld, op);
}