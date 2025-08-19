// ppanel.h - Planetarium panel package
//
// Author:  Tim Stark
// Date:    Aug 19, 2025

#pragma once

class OrbitalElements;
class Font;

class PlanetPanel : public PanelEx
{
public:
    PlanetPanel(Panel *panel);
    ~PlanetPanel();

    void draw(Player &player, Sketchpad *pad) override;

protected:
    void initResources();

    void displayPlanetocentric(Sketchpad *pad, double lat, double lng, double alt);
    void displayOrbitalElements(Sketchpad *pad, const OrbitalElements &oel, double rad);

    Font *titleFont = nullptr;
    Font *textFont = nullptr;
};