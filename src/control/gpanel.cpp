// gpanel.cpp - Generic panel package
//
// Author:  Tim Stark
// Date:    Aug 14, 2025

#include "main/core.h"
#include "api/graphics.h"
#include "engine/player.h"
#include "engine/vehicle/vehicle.h"
#include "control/mfd/panel.h"
#include "control/panel.h"
#include "control/gpanel.h"
#include "hud/panel.h"

GenericPanel::GenericPanel(Panel *panel)
: PanelEx(panel)
{
    initResources();
}

GenericPanel::~GenericPanel()
{
}

void GenericPanel::initResources()
{
    gc = panel->gc;

    width = panel->width;
    height = panel->height;
    cx = width/2;
    cy = height/2;

    brushOff = gc->createBrush({0, 1, 0, .25});
    brushOn = gc->createBrush({0, 1, 0, .5});
    hudPen = gc->createPen({0, 1, 0}, 4, 1);
    hudPen0 = gc->createPen({0, 0, 0, .5}, 4, 1);
}

void GenericPanel::render()
{
}

void GenericPanel::draw(Player &player, Sketchpad *pad)
{
    // drawButton(pad, cx/4, cy, cx/4+90, cy+60, true);
    // drawButton(pad, cx/4, cy+64, cx/4+90, cy+124, false);

    Vehicle *veh = player.getVehicleTarget();
    drawEngines(pad, veh);
}

void GenericPanel::drawButton(Sketchpad *pad, int x0, int y0, int x1, int y1, bool on)
{
    pad->setPen(hudPen);
    if (on)
        pad->setBrush(brushOn);
    else
        pad->setBrush(brushOff);
    pad->drawRectangle(x0, y0, x1, y1);
}

void GenericPanel::drawEngines(Sketchpad *pad, Vehicle *veh)
{

    double thMain = veh->getThrustGroupLevel(thgMain);
    double thRetro = veh->getThrustGroupLevel(thgRetro);
    double thHover = veh->getThrustGroupLevel(thgHover);

    pad->setPen(hudPen0);
    pad->setBrush(brushOn);
    pad->drawRectangle(150, 50, 150+(thMain*350), 100);
    pad->drawRectangle(150, 120, 150+(thRetro*350), 170);
    pad->drawRectangle(150, 190, 150+(thHover*350), 240);

    pad->setPen(hudPen);
    pad->setBrush(nullptr);
    pad->drawRectangle(150, 50, 500, 100);
    pad->drawRectangle(150, 120, 500, 170);
    pad->drawRectangle(150, 190, 500, 240);
}