// panel.h - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Aug 28, 2022

#pragma once

#include "render/hudpanel.h"

class Camera;

class Panel
{
public:
    Panel(int w, int h, int d);
    ~Panel();

    inline void switchHUDMode()     { setHUDMode(hudMode++ < HUD_MAX ? hudMode : HUD_NONE); }
    inline int getHUDMode() const   { return (hud != nullptr) ? hud->getMode() : HUD_NONE; }

    void resize(int w, int h);

    void setHUDMode(int mode);
    void drawHUD();

private:
    int width, height;  // screen size
    int depth;          // color depth

    Camera *camera = nullptr;
    HUDPanel *hud = nullptr;
    int hudMode = HUD_NONE;

};