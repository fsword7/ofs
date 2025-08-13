// panel.cpp - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "engine/player.h"
#include "utils/json.h"
#include "control/taskbar.h"
#include "control/panel.h"
#include "hud/panel.h"

Panel::Panel(GraphicsClient *gclient, int w, int h, int d)
: gc(gclient), width(w), height(h), depth(d)
{

    hudMode = HUD_NONE;
    hud = nullptr;
    for (int idx = 0; idx < HUD_MAX; idx++)
        huds[idx] = nullptr;

    if (gc != nullptr)
        bar = new TaskBar(this);

    initResources();
}

Panel::~Panel()
{
    cleanResources();
    if (bar != nullptr)
        delete bar;
}

void Panel::init(cjson &config)
{
    if (config.contains("huds")) {
        cjson &hconfig = config["huds"];
        assert(hconfig.is_array());

        for (auto &item : hconfig.items()) {
            cjson &hconfig = item.value();
            if (!hconfig.is_object())
                continue;

            HUDPanel *hud = HUDPanel::create(hconfig, gc, this);
            hudList.push_back(hud);
            int mode = hud->getMode();
            assert(mode < HUD_MAX);
            huds[mode] = hud;
            hud->resize(width, height);
        }

        // Turn HUD mode off as default
        setHUDMode(0);
    }   
}

void Panel::initResources()
{
    pad = gc->createSketchpad(nullptr);
}

void Panel::cleanResources()
{
    if (pad != nullptr)
        delete pad;
}

void Panel::resize(int w, int h)
{
    width = w;
    height = h;

    for (int idx = 0; idx < HUD_MAX; idx++)
        if (huds[idx] != nullptr)
            huds[idx]->resize(w, h);
}

void Panel::togglePanelMode()
{

}

// personal observer
void Panel::togglePersonalPanelMode()
{

}

void Panel::setPanelMode(int mode)
{
}

void Panel::toggleHUDMode()
{
    hudMode++;
    for (int idx = 0; idx < HUD_MAX; idx++, hudMode++) {
        if (hudMode == HUD_MAX)
            hudMode = 0;
        if (hudMode == 0 || huds[hudMode] != nullptr)
            break;
    }
    hud = huds[hudMode];
    // ofsLogger->info("HUD Mode: {}\n", hudMode);
}

void Panel::setHUDMode(int mode)
{
    if (mode < HUD_MAX && huds[mode] != nullptr) {
        hud = huds[mode];
        hudMode = mode;
    } else {
        hud = nullptr;
        hudMode = 0;
    }
}

void Panel::setHUDColor(color_t penColor)
{
    if (hudPen != nullptr)
        delete hudPen;
    hudPen = gc->createPen(penColor, 4, 1);
}

void Panel::update(const Player &player, double simt, double syst)
{
    if (bar != nullptr)
        bar->update(player, simt);
}

void Panel::render(const Player &player)
{
    if (bar != nullptr)
        bar->render(player);
}

void Panel::drawHUD(Player &player)
{
    if (player.isInternal()) {
        if (hud != nullptr && pad != nullptr)
            hud->draw(player, pad);
    }
}
