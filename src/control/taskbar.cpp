// taskbar.cpp - Task Bar package
//
// Author:  Tim Stark
// Date:    Feb 23, 2024

#include "main/core.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "control/panel.h"
#include "control/taskbar.h"

TaskBar::TaskBar(const Panel *panel)
: panel(panel), gc(panel->gc)
{
    initPlanetInfo();
}

TaskBar::~TaskBar()
{
    cleanPlanetInfo();
}

void TaskBar::update(const Player &player, double simt)
{
}

void TaskBar::render(const Player &player)
{

    displayPlanetInfo(player);
}