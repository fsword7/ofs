// hudpanel.h - HUD overlay screen package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

#define HUD_NONE        0               // Turn HUD off
// #define HUD_PINFO       1               // Planet information
#define HUD_SURFACE     1               // Surface HUD panel
#define HUD_ORBIT       2               // Orbit HUD panel
#define HUD_MAX         3               // Maximum number of HUD panels

class GraphicsClient;
class Sketchpad;
class Font;
class Panel;
class Vehicle;
class Player;

class HUDPanel
{
public:
    HUDPanel(const Panel *panel);

    static HUDPanel *create(cjson &config, GraphicsClient *gc, Panel *panel);

    void resize(int w, int h);

    inline virtual int getMode() const = 0;

    void draw(Sketchpad *pad);

    void drawDefault(Sketchpad *pad);

protected:
    virtual void configure(cjson &config) = 0;
    virtual void display(Sketchpad *pad) = 0;

    virtual void drawLadderBar(Sketchpad *pad, double lcosa, double lsina,
        double dcosa, double dsina, double phi0);
    virtual void drawCompassRibbon(Sketchpad *pad, double val);

    const Vehicle *vehicle = nullptr;
    
    int lwidth; // ladder width
    int lrange; // ladder range

    const Panel *panel;

    GraphicsClient *gc = nullptr;

    int width, height;
    int hres05, vres05;
    int cx, cy;

    Font *titleFont = nullptr;
    Font *textFont = nullptr;
};

class HUDSurfacePanel : public HUDPanel
{
public:
    HUDSurfacePanel(const Panel *panel);
    ~HUDSurfacePanel();

    inline int getMode() const override { return HUD_SURFACE; }

    void configure(cjson &config) override;

protected:
    void display(Sketchpad *pad) override;

};

class HUDOrbitPanel : public HUDPanel
{
public:
    HUDOrbitPanel(const Panel *panel);
    ~HUDOrbitPanel();

    inline int getMode() const override { return HUD_ORBIT; }

    void configure(cjson &config) override;

protected:
    void display(Sketchpad *pad) override;

};