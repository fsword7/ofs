// engine.h - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class Scene;
class Player;
class Universe;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

    inline Scene *getScene()       { return scene; }
    inline Universe *getUniverse() { return universe; }
    inline Player *getPlayer()     { return player; }
    inline Date *getDate()         { return &realDate; }

    inline double getRealTime()    { return realTime; }
    
    void init(Context *ctx, int width, int height);

    void start();
    void update(double dt);
    void render();

private:
    Universe *universe = nullptr;
    Player *player = nullptr;

    Date    realDate;
    double  realTime;
    double  scaleTime = 1.0;

    // Renderer
    Scene *scene = nullptr;
};