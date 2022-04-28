// engine.h - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class TextureFont;
class Overlay;
class Scene;
class Player;
class Universe;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

    inline Scene *getScene()        { return scene; }
    inline Universe *getUniverse()  { return universe; }
    inline Player *getPlayer()      { return player; }
    inline Date *getDate()          { return &realDate; }

    inline double getRealTime()     { return realTime; }
    inline double getTimeWarp()     { return scaleTime; }

    inline void setTimeWarp(double scale)   { scaleTime = scale; }
    
    void init(Context *ctx, int width, int height);

    void start();
    void update(double dt);
    void render();

    void renderOverlay();
    void displayPlanetInfo();
    void displayPlanetocentric(double lon, double lat, double alt);

private:
    Universe *universe = nullptr;
    Player *player = nullptr;

    Date    realDate;
    double  realTime;
    double  scaleTime = 1.0;

    // Renderer
    Scene *scene = nullptr;

    // Overlay
    TextureFont *titleFont = nullptr;
    TextureFont *textFont = nullptr;
    Overlay *overlay = nullptr;
    vec3d_t lastView;
};