// vobject.cpp - Visual Object Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/mesh.h"
#include "render/scene.h"
#include "render/vbody.h"

vBody::vBody(const Object &obj, Scene &scene)
: vObject(obj, scene)
{ 
    smgr = new SurfaceManager(scene.getContext(), obj);
}

void vBody::render(renderParam &prm, ObjectProperties &op, LightState &lights)
{
    smgr->render(prm, op);
}