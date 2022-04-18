// vvessel.h - Visual Vessel (Spacecraft) Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#pragma once

#include "render/vobject.h"

class vVessel : public vObject
{
public:
    vVessel(const Object &obj, Scene &scene)
    : vObject(obj, scene)
    { }
    ~vVessel() = default;

    void render() override;
    
private:
};