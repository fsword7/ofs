// vbody.h - Visual Celestial Body package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#pragma once

class SurfaceManager;

class vBody : public vObject
{
public:
    vBody(const Object *object, Scene &scene);
    ~vBody();

    void update(int now) override;
    void render(const ObjectListEntry &ole) override;

private:
    SurfaceManager *smgr = nullptr;
};