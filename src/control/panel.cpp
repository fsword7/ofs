// ipanel.cpp - Instrumental panel package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "engine/player.h"
#include "utils/json.h"
#include "control/taskbar.h"
#include "control/panel.h"
#include "control/ppanel.h"
#include "control/gpanel.h"
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
        setHUDMode(HUD_SURFACE);
    }

    // initialize internal panels
    createPanel(PANEL_PLANET);
    createPanel(PANEL_GENERIC);

    // Generic panel mode as default
    setPanelMode(PANEL_GENERIC); 
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

// personal observer
void Panel::togglePersonalPanelMode()
{

}


void Panel::createPanel(int mode)
{
    switch (mode)
    {
    case PANEL_PLANET:
        panels[PANEL_PLANET] = new PlanetPanel(this);
        break;
    case PANEL_GENERIC:
        panels[PANEL_GENERIC] = new GenericPanel(this);
        break;
    }
}

void Panel::setPanelMode(int mode)
{
    if (mode < PANEL_MAX && panels[mode] != nullptr) {
        panel = panels[mode];
        panelMode = mode;
    } else {
        panel = nullptr;
        panelMode = PANEL_NONE;
    }
}

void Panel::togglePanelMode()
{
    panelMode++;
    for (int idx = 0; idx < PANEL_MAX; idx++, panelMode++) {
        if (panelMode == PANEL_MAX)
            panelMode = 0;
        if (panelMode == 0 || panels[panelMode] != nullptr)
            break;
    }
    panel = panels[panelMode];
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

void Panel::toggleHUD()
{
    hudEnable = !hudEnable;
}

void Panel::switchHUDMode()
{
    hudMode++;
    for (int idx = 0; idx < HUD_MAX; idx++, hudMode++) {
        if (hudMode == HUD_MAX)
            hudMode = 0;
        if (hudMode == 0 || huds[hudMode] != nullptr)
            break;
    }
    hud = huds[hudMode];
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
    if (player.isExternal())
        return;

    switch (panelMode) {
    case PANEL_NONE:
        if (hudEnable && hud != nullptr)
            hud->draw(player, pad);
        break;

    case PANEL_PLANET:
        if (panel == nullptr || pad == nullptr)
            break;
        panel->draw(player, pad);
        break;

    case PANEL_GENERIC:
        if (panel == nullptr || pad == nullptr)
            break;
        if (hudEnable && hud != nullptr)
            hud->draw(player, pad);
        panel->draw(player, pad);
        break;
    }
}
