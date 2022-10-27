// panel.cpp - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Aug 28, 2022

#include "main/core.h"
#include "engine/player.h"
#include "engine/panel.h"

Panel::Panel(int w, int h, int d)
: width(w), height(h), depth(d)
{

}

Panel::~Panel()
{

}

void Panel::resize(int w, int h)
{
    width = w;
    height = h;
}

void Panel::setHUDMode(int mode)
{
    if (hud != nullptr)
        delete hud;

    hudMode = mode;
    switch (mode)
    {
    case HUD_SURFACE:
        // hud = new HUDSurfacePanel(this);
        break;
    case HUD_ORBIT:
        // hud = new HUDOrbitPanel(this);
        break;
    default:
        hud = nullptr;
    }
}

void Panel::drawHUD()
{
    if (hud != nullptr && !camera->isExternal())
    {
        Sketchpad *pad = nullptr; // gc->getSketchpad();
        if (pad != nullptr)
        {
            // hud->draw(pad);
            // gc->releaseSketchpad();
        }
    }
}
