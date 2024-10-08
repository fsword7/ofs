// panel.h - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

#include "control/hud/panel.h"
// #include "control/mfd/panel.h"

class GraphicsClient;
class Player;

class TaskBar;

class Panel
{
    friend class TaskBar;

public:
    Panel(GraphicsClient *gclient, int w, int h, int d);
    ~Panel();

    void initResources();
    void cleanResources();

    inline void switchHUDMode()     { setHUDMode(hudMode++ < HUD_MAX ? hudMode : HUD_NONE); }
    inline int getHUDMode() const   { return (hud != nullptr) ? hud->getMode() : HUD_NONE; }

    void resize(int w, int h);
    void update(const Player &player, double simt, double syst);
    void render(const Player &player);

    void setHUDMode(int mode);
    void drawHUD();

private:
    int width, height;  // screen size
    int depth;          // color depth

    GraphicsClient *gc = nullptr;
    // Camera *camera = nullptr;
    HUDPanel *hud = nullptr;
    int hudMode = HUD_NONE;

    TaskBar *bar = nullptr;
};