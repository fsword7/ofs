// hudpanel.h - HUD overlay screen package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

#define HUD_NONE        0               // Turn HUD off
#define HUD_SURFACE     1               // Surface HUD panel
#define HUD_ORBIT       2               // Orbit HUD panel
#define HUD_MAX         HUD_ORBIT       // Maximum number of HUD panels

class Sketchpad;
class Panel;

class HUDPanel
{
public:
    HUDPanel(const Panel *panel);

    inline virtual int getMode() const = 0;

    void draw(Sketchpad *pad);

protected:
    virtual void display() = 0;

    void drawLadderBar();
    void drawCompassRibbon();

private:
    const Panel *panel;

};

class HUDSurfacePanel : public HUDPanel
{
public:
    HUDSurfacePanel(const Panel *panel);

    inline int getMode() const override { return HUD_SURFACE; }

protected:
    void display() override;

};

class HUDOrbitPanel : public HUDPanel
{
public:
    HUDOrbitPanel(const Panel *panel);

    inline int getMode() const override { return HUD_ORBIT; }

protected:
    void display() override;

};