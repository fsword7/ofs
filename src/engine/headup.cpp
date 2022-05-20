// headup.cpp - Overlay information display package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/shader.h"
#include "engine/engine.h"
#include "engine/object.h"
#include "engine/player.h"
// #include "engine/headup.h"
#include "universe/body.h"
#include "render/overlay.h"

void Engine::renderOverlay()
{
    double curTime = player->getJulianTime();

    color_t color(0.7, 0.7, 1.0, 1.0);

    overlay->home();
    overlay->moveBy(15.0, 15.0);
    overlay->setColor(color);

    displayPlanetInfo();

}

void Engine::displayPlanetocentric(double lng, double lat, double alt)
{
    char lngHemi, latHemi;

    lngHemi = lng < 0.0 ? 'W' : lng > 0.0 ? 'E' : ' ';
    latHemi = lat < 0.0 ? 'S' : lat > 0.0 ? 'N' : ' ';

    double dlng = abs(glm::degrees(lng));
    double dlat = abs(glm::degrees(lat));

    str_t locObject = fmt::format("Latitude/Longiude: {}{} {}{}",
        dlat, latHemi, dlng, lngHemi);
    overlay->print(locObject);
}

void Engine::displayPlanetInfo()
{
    const Object *focus = getFoucsedObject();
    double jdTime = player->getJulianTime();
    vec3d_t view = focus->getuPosition(jdTime) - player->getuPosition();

    overlay->setFont(titleFont);
    overlay->print(focus->getsName());

    overlay->setFont(textFont);
    str_t distObject = fmt::format("Distance: {}", glm::length(view));
    str_t radiusObject = fmt::format("Radius: {}", focus->getRadius());

    overlay->print(distObject);
    overlay->print(radiusObject);

    // const vec3d_t opos = center->getoPosition(curTime);
    vec3d_t pc = dynamic_cast<const celBody *>(focus)->getPlanetocentricFromEcliptic(view, jdTime);
    displayPlanetocentric(pc.x, pc.y, pc.z);
}