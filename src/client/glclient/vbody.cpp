// vbody.cpp - Visual Celestial Body package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#include "main/core.h"
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