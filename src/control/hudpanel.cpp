// hudpanel.cpp - HUD overlay screen package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "control/panel.h"
#include "control/hudpanel.h"

HUDPanel::HUDPanel(const Panel *panel)
: panel(panel)
{

    gc = ofsAppCore->getClient();

    titleFont = gc->createFont("Arial", 20, false, Font::Bold);
    textFont = gc->createFont("Arial", 12, false);
}

void HUDPanel::draw(Sketchpad *pad)
{

}

void HUDPanel::drawCompassRibbon()
{

}

void HUDPanel::drawLadderBar()
{

}

// ******** HUD Surface Panel ********

HUDSurfacePanel::HUDSurfacePanel(const Panel *panel)
: HUDPanel(panel)
{

}

void HUDSurfacePanel::display()
{

}

// ******** HUD Orbit Panel ********

HUDOrbitPanel::HUDOrbitPanel(const Panel *panel)
: HUDPanel(panel)
{
    
}

void HUDOrbitPanel::display()
{

}