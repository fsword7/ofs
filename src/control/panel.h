// panel.h - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

#include "control/hudpanel.h"

class Camera;
class GraphicsClient;

class Panel
{
public:
    Panel(GraphicsClient *gclient, int w, int h, int d);
    ~Panel();

    void initResources();
    void cleanResources();

    inline void switchHUDMode()     { setHUDMode(hudMode++ < HUD_MAX ? hudMode : HUD_NONE); }
    inline int getHUDMode() const   { return (hud != nullptr) ? hud->getMode() : HUD_NONE; }

    void resize(int w, int h);

    void setHUDMode(int mode);
    void drawHUD();

    void displayPlanetocentric(double lat, double lng, double alt);
    void displayPlanetInfo(const Player &player);

private:
    int width, height;  // screen size
    int depth;          // color depth

    GraphicsClient *gc = nullptr;
    // Camera *camera = nullptr;
    HUDPanel *hud = nullptr;
    int hudMode = HUD_NONE;

    Font *titleFont = nullptr;
    Font *textFont = nullptr;
};