// taskbar.cpp - Task Bar package
//
// Author:  Tim Stark
// Date:    Feb 23, 2024

#include "main/core.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "ephem/elements.h"
#include "control/panel.h"
#include "control/taskbar.h"
#include "engine/player.h"

TaskBar::TaskBar(const Panel *panel)
: panel(panel), gc(panel->gc)
{
    ipad = gc->createSketchpad(nullptr);
    titleFont = gc->createFont("Arial", 80, false, Font::Bold);
    textFont = gc->createFont("Arial", 30, false);

    resize();

    initPlanetInfo();

}

TaskBar::~TaskBar()
{
    if (ipad != nullptr)
        delete ipad;
    if (titleFont != nullptr)
        delete titleFont;
    if (textFont != nullptr)
        delete textFont;

    cleanPlanetInfo();
}

void TaskBar::resize()
{
    width = ipad->getWidth();
    height = ipad->getHeight();
}

void TaskBar::update(const Player &player, double simt)
{
}

void TaskBar::render(const Player &player)
{
    int xofs = (width / 4) * 3;
    color_t col = { 0.40, 0.60, 1.0, 1.0 };

    ipad->beginDraw();
    ipad->setFont(textFont);
    ipad->setTextColor(col);
    ipad->setTextPos(xofs, 3);

    ipad->print(astro::getMJDDateStr(ofsDate->getMJD1()));
    ipad->print(fmt::format("MJD {:.5f}  ({}x)",
        ofsDate->getMJD1(), ofsDate->getTimeWarp()));

    ipad->endDraw();

    // displayPlanetInfo(player);
}