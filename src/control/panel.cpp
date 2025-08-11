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
        }

        // Turn HUD mode off as default
        setHUDMode(0);
    }   
}

void Panel::initResources()
{

}

void Panel::cleanResources()
{

}

void Panel::resize(int w, int h)
{
    width = w;
    height = h;
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

void Panel::drawHUD(const Player &player)
{
    if (player.isInternal()) {
        // Sketchpad *pad = gc->getSketchpad();
        // if (hud != nullptr && pad != nullptr)
        //     hud->draw(player, pad);
    }
}
