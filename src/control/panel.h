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

    void init(cjson &config);

    inline void switchHUDMode()     { setHUDMode(hudMode++ < HUD_MAX ? hudMode : HUD_NONE); }
    inline int getHUDMode() const   { return (hud != nullptr) ? hud->getMode() : HUD_NONE; }

    void togglePanelMode();
    void togglePersonalPanelMode();
    void setPanelMode(int mode);

    void resize(int w, int h);
    void update(const Player &player, double simt, double syst);
    void render(const Player &player);

    void toggleHUDMode();
    void setHUDMode(int mode);
    void drawHUD(const Player &player);

private:
    int width, height;  // screen size
    int depth;          // color depth

    GraphicsClient *gc = nullptr;
    // Camera *camera = nullptr;
    int hudMode = HUD_NONE;
    HUDPanel *hud = nullptr;

    std::vector<HUDPanel *> hudList;
    HUDPanel *huds[HUD_MAX];

    TaskBar *bar = nullptr;
};