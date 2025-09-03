// ipanel.h - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

#include "control/hud/panel.h"
// #include "control/mfd/panel.h"

#define PANEL_NONE      0
#define PANEL_PLANET    1
#define PANEL_GENERIC   2
#define PANEL_MAX       3

class GraphicsClient;
class Player;

class TaskBar;
class Panel;
class PanelEx;
class GenericPanel;

class Panel
{
    friend class TaskBar;
    friend class PanelEx;
    friend class GenericPanel;
    friend class PlanetPanel;

public:
    Panel(GraphicsClient *gclient, int w, int h, int d);
    ~Panel();

    void initResources();
    void cleanResources();

    void init(cjson &config);

    // inline void switchHUDMode()     { setHUDMode(hudMode++ < HUD_MAX ? hudMode : HUD_NONE); }
    inline int getHUDMode() const   { return (hud != nullptr) ? hud->getMode() : HUD_NONE; }
    inline Pen *getHUDPen() const   { return hudPen; }

    void togglePanelMode();
    void togglePlanetariumPanelMode();
    void togglePersonalPanelMode();
    void createPanel(int mode);
    void setPanelMode(int mode);
    void setHUDColor(color_t color);

    void resize(int w, int h);
    void update(const Player &player, double simt, double syst);
    void render(const Player &player);

    void toggleHUD();
    void switchHUDMode();
    void setHUDMode(int mode);
    void drawHUD(Player &player);

private:
    int width, height;  // screen size
    int depth;          // color depth

    GraphicsClient *gc = nullptr;
    Sketchpad *pad = nullptr;
    // Camera *camera = nullptr;

    int panelMode = 0;
    GenericPanel *gpanel = nullptr;
    PanelEx *panels[PANEL_MAX];
    PanelEx *panel = nullptr; 

    bool hudEnable = true;
    int hudMode = HUD_NONE;
    HUDPanel *hud = nullptr;

    Pen *hudPen = nullptr;

    std::vector<HUDPanel *> hudList;
    HUDPanel *huds[HUD_MAX];

    TaskBar *bar = nullptr;
};

class PanelEx
{
public:
    PanelEx(Panel *panel)
    : panel(panel)
    { 
        gc = panel->gc;
    }

    virtual void draw(Player &player, Sketchpad *pad) {};

protected:
    Panel *panel = nullptr;
    GraphicsClient *gc = nullptr;

};

