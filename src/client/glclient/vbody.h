// vbody.h - Visual Celestial Body package
//
// Author:  Tim Stark
// Date:    Sep 13, 2022

#pragma once

class SurfaceManager;

class vBody : public vObject
{
public:
    vBody(ObjectHandle object, Scene &scene);
    ~vBody();

    void update(int now) override;
    void render(const ObjectProperties &op) override;

private:
    SurfaceManager *smgr = nullptr;
};