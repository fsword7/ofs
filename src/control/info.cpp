// headup.cpp - Overlay information display package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#include "main/core.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "control/taskbar.h"
#include "ephem/elements.h"
#include "engine/celestial.h"
#include "engine/vehicle/vehicle.h"
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
    // if (ipad != nullptr)
    //     delete ipad;
    // if (titleFont != nullptr)
    //     delete titleFont;
    // if (textFont != nullptr)
    //     delete textFont;
}

void TaskBar::displayPlanetocentric(double lat, double lng, double alt)
{
    char latHemi, lngHemi;

    latHemi = lat < 0.0 ? 'S' : lat > 0.0 ? 'N' : ' ';
    lngHemi = lng < 0.0 ? 'W' : lng > 0.0 ? 'E' : ' ';

    double dlat = fabs(ofs::degrees(lat));
    double dlng = fabs(ofs::degrees(lng));

    ipad->print(fmt::format("Location: {:.6f}{} {:.6f}{}",
        dlat, latHemi, dlng, lngHemi));
}

void TaskBar::displayOrbitalElements(const OrbitalElements &oel, double rad)
{
    double alt = glm::length(oel.getoPosition()) / M_PER_KM;
    double ad = oel.getApoapsisDistance() / M_PER_KM;
    double pd = oel.getPeriapsisDistance() / M_PER_KM;
    double major = oel.getSemiMajorAxis() / M_PER_KM;
    double minor = oel.getSemiMinorAxis() / M_PER_KM;

    ipad->print("---- orbital elements ----");
    ipad->print(fmt::format("Altitude:          {:.4f} km", alt - rad));
    ipad->print(fmt::format("Apoapsis:          {:.4f} km", ad - rad));
    ipad->print(fmt::format("Periapsos:         {:.4f} km", pd - rad));
    ipad->print(fmt::format("Semi-Major Axis:   {:.4f} km", major - rad));
    ipad->print(fmt::format("Semi-Minor Axis:   {:.4f} km", minor - rad));
    ipad->print(fmt::format("Eccentricity:      {:.6f}", oel.getEccentricity()));
    ipad->print(fmt::format("Inclination:       {:.2f}", ofs::degrees(oel.getInclination())));
    ipad->print(fmt::format("Long of asc node:  {:.2f}", ofs::degrees(oel.getLongitudeOfAcendingNode())));
    ipad->print(fmt::format("Long of perapsis:  {:.2f}", ofs::degrees(oel.getLongitudeOfPerapsis())));
    ipad->print(fmt::format("Mean longitude:    {:.2f}", ofs::degrees(oel.getMeanLongitude())));
    ipad->print(fmt::format("Orbital Period:    {:.2f} min", oel.getOrbitalPeriod() / 60));
    ipad->print("--------------------------");
}

void TaskBar::displayPlanetInfo(const Player &player)
{
    color_t col = { 0.40, 0.60, 1.0, 1.0 };

    ipad->beginDraw();
    ipad->setFont(titleFont);
    ipad->setTextColor(col);
    ipad->setTextPos(5, 3);
    
    const Celestial *focus = player.getReferenceObject();
    const Vehicle *veh = nullptr;
    assert(focus != nullptr);
    if (focus->getType() == objVehicle) {
        veh = dynamic_cast<const Vehicle *>(focus);
        focus = veh->getOrbitalReference();
    }
    ipad->print(focus->getsName());

    ipad->setFont(textFont);
    ipad->print(fmt::format("Distance: {:.3f} miles", glm::length(focus->getgPosition()) / 1.609));
    ipad->print(fmt::format("Velocity: {:.3f} mph", ((glm::length(focus->getgVelocity()) * 3600) / 1.609)));
    ipad->print(fmt::format("Radius: {}", focus->getRadius()));
    ipad->print("-----------------");
    if (veh != nullptr) {
        ipad->print(fmt::format("Vehicle: {} - {}",
            veh->getsName(), veh->getsName(1)));
        ipad->print(fmt::format("Reference: {}",
            veh->getOrbitalReference()->getsName()));
        if (veh->isOrbitalValid())
            displayOrbitalElements(veh->getOrbitalElements(), focus->getRadius());
        ipad->print(fmt::format("Ground Velocity: {:.3f} mph",
            ((glm::length(veh->getgVelocity()) * 3600) / 1.609)));
        ipad->print(fmt::format("Altitude (MSL): {:.3f} ft ({:.3f} m)",
            veh->getAltitudeMSL() * 3280.84, veh->getAltitudeMSL() * 1000.0));
        ipad->print(fmt::format("Altitude (AGL): {:.3f} ft ({:.3f} m)",
            veh->getAltitudeAGL() * 3280.84, veh->getAltitudeAGL() * 1000.0));       
    }
    ipad->print(fmt::format("Distance: {:.4f}", glm::length(player.getrPosition())));

    glm::dvec3 lpos = focus->convertGlobalToLocal(player.getPosition());
    glm::dvec3 loc = focus->convertLocalToEquatorial(lpos);
    displayPlanetocentric(loc.x, loc.y, loc.z);

    ipad->endDraw();
}