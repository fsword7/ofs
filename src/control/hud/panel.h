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
class Celestial;
class Vehicle;
class Player;
class Camera;

class HUDPanel
{
public:
    HUDPanel(Panel *panel);

    static HUDPanel *create(cjson &config, GraphicsClient *gc, Panel *panel);

    void resize(int w, int h);

    inline virtual int getMode() const = 0;

    void draw(Player &player, Sketchpad *pad);

    void drawDefault(Sketchpad *pad);

protected:
    virtual void configure(cjson &config) = 0;
    virtual void display(Player &player, Sketchpad *pad) = 0;

    bool checkVisualArea(const glm::dvec3 &gpos, glm::dvec3 &vscr);
    
    bool getXY(Celestial *obj, const glm::dvec3 &dir, int &x, int &y);
    bool getXY(Celestial *obj, const glm::dvec3 &dir, double &x, double &y);

    virtual void drawLadderBar(Sketchpad *pad, double lcosb, double lsinb,
        double dcosb, double dsinb, double phi0, bool markSubzero);
    virtual void drawCompassRibbon(Sketchpad *pad, double val);
    virtual bool drawMarker(Sketchpad *pad, Celestial *obj, const glm::dvec3 &dir, int style);
    virtual bool drawOffscreenDirMarker(Sketchpad *pad, const glm::dvec3 &dir);

    const Vehicle *vehicle = nullptr;
    
    int lwidth; // ladder width
    int lrange; // ladder range
    glm::dvec3 hudofs;

    Panel *panel;
    Camera *cam = nullptr;

    GraphicsClient *gc = nullptr;

    int width, height;
    int hres05, vres05;
    int cx, cy;
    int markerSize;

    Font *hudFont = nullptr;
};

class HUDSurfacePanel : public HUDPanel
{
public:
    HUDSurfacePanel(Panel *panel);
    ~HUDSurfacePanel();

    inline int getMode() const override { return HUD_SURFACE; }

    void configure(cjson &config) override;

protected:
    void display(Player &platyer, Sketchpad *pad) override;

};

class HUDOrbitPanel : public HUDPanel
{
public:
    HUDOrbitPanel(Panel *panel);
    ~HUDOrbitPanel();

    inline int getMode() const override { return HUD_ORBIT; }

    void configure(cjson &config) override;

protected:
    void display(Player &player, Sketchpad *pad) override;

};