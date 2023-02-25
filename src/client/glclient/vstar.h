// vstar.h - Visual Celestial Star package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#pragma once

class SurfaceManager;

class vStar : public vObject
{
public:
    vStar(const Object *object, Scene &scene);
    ~vStar();
    
    void update(int now) override;
    void render(const ObjectProperties &op) override;

private:
    SurfaceManager *smgr = nullptr;
};