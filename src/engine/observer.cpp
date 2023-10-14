// observer.cpp - Observer (player/camera) package
//
// Author:  Tim Stark
// Date:    Oct 13, 2023

#include "main/core.h"
#include "engine/object.h"
#include "engine/observer.h"

Observer::Observer(int w, int h)
: width(w), height(h)
{
    fov   = glm::radians(SCR_FOV);
    zNear = 0.0001;
    zFar  = 1e24;

    resize(width, height);
}

void Observer::resize(int w, int h)
{
    width  = w;
    height = h;
    aspect = (double)width / (double)height;

}

void Observer::attach(Object *object, cameraMode mode)
{

    tgtObject = object;

}

void Observer::update(const TimeDate &td)
{

    switch (modeCamera)
    {
    case camGlobalFrame:
        gpos = rpos;
        grot = rrot;
        break;
    case camTargetRelative:
        gspos = tgtObject->getuOrientation(0) * rpos;
        gpos = tgtObject->getoPosition() + gspos;
        grot = tgtObject->getuOrientation(0) * rrot;
        break;
    }
}