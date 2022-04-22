// vbody.h - Visual Celestial Body Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#pragma once

#include "render/surface.h"
#include "render/vobject.h"

class vBody : public vObject
{
public:
    vBody(const Object &obj, Scene &scene);
    ~vBody() = default;

    void render(renderParam &prm, ObjectProperties &op) override;
    
private:
    SurfaceManager *smgr = nullptr;
};