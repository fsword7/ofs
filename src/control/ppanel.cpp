// ppanel.cpp - Planetarium panel package
//
// Author:  Tim Stark
// Date:    Aug 19, 2025

#include "main/core.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "control/panel.h"
#include "control/ppanel.h"
#include "ephem/elements.h"
#include "engine/player.h"
#include "engine/vehicle/vehicle.h"

PlanetPanel::PlanetPanel(Panel *panel)
: PanelEx(panel)
{
    initResources();
}

PlanetPanel::~PlanetPanel()
{

}

void PlanetPanel::initResources()
{
    titleFont = gc->createFont("Arial", 80, false, Font::Bold);
    textFont = gc->createFont("Arial", 30, false);
}

void PlanetPanel::displayPlanetocentric(Sketchpad *pad, double lat, double lng, double alt)
{
    char latHemi, lngHemi;

    latHemi = lat < 0.0 ? 'S' : lat > 0.0 ? 'N' : ' ';
    lngHemi = lng < 0.0 ? 'W' : lng > 0.0 ? 'E' : ' ';

    double dlat = fabs(ofs::degrees(lat));
    double dlng = fabs(ofs::degrees(lng));

    pad->print(fmt::format("Location: {:.6f}{} {:.6f}{}",
        dlat, latHemi, dlng, lngHemi));
}

void PlanetPanel::displayOrbitalElements(Sketchpad *pad, const OrbitalElements &oel, double rad)
{
    glm::dvec3 pos = oel.getoPosition() / M_PER_KM;
    double alt = glm::length(pos);
    double ad = oel.getApoapsisDistance() / M_PER_KM;
    double pd = oel.getPeriapsisDistance() / M_PER_KM;
    double major = oel.getSemiMajorAxis() / M_PER_KM;
    double minor = oel.getSemiMinorAxis() / M_PER_KM;

    pad->print("---- orbital elements ----");
    // pad->print(fmt::format("Position: {:.4f},{:.4f},{:.4f}", pos.x, pos.y, pos.z));
    pad->print(fmt::format("Altitude:          {:.4f} km", alt - rad));
    pad->print(fmt::format("Apoapsis:          {:.4f} km", ad - rad));
    pad->print(fmt::format("Periapsos:         {:.4f} km", pd - rad));
    pad->print(fmt::format("Semi-Major Axis:   {:.4f} km", major - rad));
    pad->print(fmt::format("Semi-Minor Axis:   {:.4f} km", minor - rad));
    pad->print(fmt::format("Eccentricity:      {:.6f}", oel.getEccentricity()));
    pad->print(fmt::format("Inclination:       {:.2f}", ofs::degrees(oel.getInclination())));
    pad->print(fmt::format("Long of asc node:  {:.2f}", ofs::degrees(oel.getLongitudeOfAcendingNode())));
    pad->print(fmt::format("Long of perapsis:  {:.2f}", ofs::degrees(oel.getLongitudeOfPerapsis())));
    pad->print(fmt::format("Mean longitude:    {:.2f}", ofs::degrees(oel.getMeanLongitude())));
    pad->print(fmt::format("Orbital Period:    {:.2f} min", oel.getOrbitalPeriod() / 60));
    pad->print("--------------------------");
}

void PlanetPanel::draw(Player &player, Sketchpad *pad)
{
    color_t col = { 0.40, 0.60, 1.0, 1.0 };

    pad->beginDraw();
    pad->setFont(titleFont);
    pad->setTextColor(col);
    pad->setTextPos(5, 3);
    
    const Celestial *focus = player.getReferenceObject();
    const Vehicle *veh = nullptr;
    assert(focus != nullptr);
    if (focus->getType() == objVehicle) {
        veh = dynamic_cast<const Vehicle *>(focus);
        focus = veh->getOrbitalReference();
    }
    pad->print(focus->getsName());

    pad->setFont(textFont);
    pad->print(fmt::format("Distance: {:.3f} miles", glm::length(focus->getgPosition()) / 1.609));
    pad->print(fmt::format("Velocity: {:.3f} mph", ((glm::length(focus->getgVelocity()) * 3600) / 1.609)));
    pad->print(fmt::format("Radius: {}", focus->getRadius()));
    pad->print("-----------------");
    if (veh != nullptr) {
        pad->print(fmt::format("Vehicle: {} - {}",
            veh->getsName(), veh->getsName(1)));
        pad->print(fmt::format("Reference: {}",
            veh->getOrbitalReference()->getsName()));
        if (veh->isOrbitalValid())
            displayOrbitalElements(pad, veh->getOrbitalElements(), focus->getRadius());
        pad->print(fmt::format("Ground Velocity: {:.3f} mph",
            ((glm::length(veh->getgVelocity()) * 3600) / 1.609)));
        pad->print(fmt::format("Altitude (MSL): {:.3f} ft ({:.3f} m)",
            veh->getAltitudeMSL() * 3280.84, veh->getAltitudeMSL() * 1000.0));
        pad->print(fmt::format("Altitude (AGL): {:.3f} ft ({:.3f} m)",
            veh->getAltitudeAGL() * 3280.84, veh->getAltitudeAGL() * 1000.0));       
    }
    pad->print(fmt::format("Distance: {:.4f}", glm::length(player.getrPosition())));

    glm::dvec3 lpos = focus->convertGlobalToLocal(player.getPosition());
    glm::dvec3 loc = focus->convertLocalToEquatorial(lpos);
    displayPlanetocentric(pad, loc.x, loc.y, loc.z);

    pad->endDraw();
}