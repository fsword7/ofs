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

private:
    SurfaceManager *smgr = nullptr;
};