// headup.cpp - Overlay information display package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#include "main/core.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "control/taskbar.h"
#include "engine/celestial.h"
#include "engine/player.h"

// void Engine::renderOverlay()
// {
//     // double curTime = player->getJulianTime();

//     // color_t color(0.7, 0.7, 1.0, 1.0);

//     // overlay->home();
//     // overlay->moveBy(15.0, 15.0);
//     // overlay->setColor(color);

//     // displayPlanetInfo();

// }

void TaskBar::initPlanetInfo()
{
    // ipad = gc->createSketchpad(nullptr);
    // titleFont = gc->createFont("Arial", 80, false, Font::Bold);
    // textFont = gc->createFont("Arial", 30, false);
}

void TaskBar::cleanPlanetInfo()
{
    if (ipad != nullptr)
        delete ipad;
    if (titleFont != nullptr)
        delete titleFont;
    if (textFont != nullptr)
        delete textFont;
}

void TaskBar::displayPlanetocentric(double lat, double lng, double alt)
{
    char latHemi, lngHemi;

    latHemi = lat < 0.0 ? 'S' : lat > 0.0 ? 'N' : ' ';
    lngHemi = lng < 0.0 ? 'W' : lng > 0.0 ? 'E' : ' ';

    double dlat = abs(ofs::degrees(lat));
    double dlng = abs(ofs::degrees(lng));

    ipad->print(fmt::format("Location: {:.6f}{} {:.6f}{}",
        dlat, latHemi, dlng, lngHemi));
 }

void TaskBar::displayPlanetInfo(const Player &player)
{
    color_t col = { 0.40, 0.60, 1.0, 1.0 };

    ipad->beginDraw();
    ipad->setFont(titleFont);
    ipad->setTextColor(col);
    ipad->setTextPos(5, 3);
    
    const Celestial *focus = player.getReferenceObject();
    assert(focus != nullptr);
    ipad->print(focus->getsName());

    ipad->setFont(textFont);
    ipad->print(fmt::format("Distance: {:.3f} miles", glm::length(focus->getoPosition()) / 1.609));
    ipad->print(fmt::format("Velocity: {:.3f} mph", ((glm::length(focus->getoVelocity()) * 3600) / 1.609)));
    ipad->print(fmt::format("Radius: {}", focus->getRadius()));
    ipad->print("-----------------");
    ipad->print(fmt::format("Distance: {:.4f}", glm::length(player.getrPosition())));

    glm::dvec3 lpos = focus->convertGlobalToLocal(player.getPosition());
    glm::dvec3 loc = focus->convertLocalToEquatorial(lpos);
    displayPlanetocentric(loc.x, loc.y, loc.z);

    ipad->endDraw();
}