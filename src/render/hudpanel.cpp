// hudpanel.cpp - HUD overlay screen package
//
// Author:  Tim Stark
// Date:    Aug 28, 2022

#include "main/core.h"
#include "engine/panel.h"
#include "api/draw.h"
#include "render/hudpanel.h"

HUDPanel::HUDPanel(const Panel *panel)
: panel(panel)
{

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