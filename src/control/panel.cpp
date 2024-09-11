// panel.cpp - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "engine/player.h"
#include "control/taskbar.h"
#include "control/panel.h"

Panel::Panel(GraphicsClient *gclient, int w, int h, int d)
: gc(gclient), width(w), height(h), depth(d)
{

    hudMode = HUD_NONE;
    hud = nullptr;

    if (gc != nullptr)
        bar = new TaskBar(this);

    initResources();
}

Panel::~Panel()
{
    cleanResources();
    if (bar != nullptr)
        delete bar;
}

void Panel::initResources()
{

}

void Panel::cleanResources()
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
    // case HUD_PINFO:
    default:
        hud = nullptr;
    }
}

void Panel::update(const Player &player, double simt, double syst)
{
    if (bar != nullptr)
        bar->update(player, simt);
}

void Panel::render(const Player &player)
{
    if (bar != nullptr)
        bar->render(player);
}

void Panel::drawHUD()
{
    // if (hud != nullptr && !camera->isExternal())
    // {
    //     Sketchpad *pad = nullptr; // gc->getSketchpad();
    //     if (pad != nullptr)
    //     {
    //         // hud->draw(pad);
    //         // gc->releaseSketchpad();
    //     }
    // }
}
