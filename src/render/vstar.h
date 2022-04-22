// vstar.h - Visual Celestial Star Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#pragma once

#include "render/surface.h"
#include "render/vobject.h"

class vStar  : public vObject
{
public:
    vStar(const Object &obj, Scene &scene);
    ~vStar() = default;

    void render(renderParam &prm, ObjectProperties &op) override;
    
private:
    SurfaceManager *smgr = nullptr;
};