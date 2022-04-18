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
    
    void init(Context *ctx, int width, int height);

    void start();
    void update(double dt);
    void render();

private:
    Universe *universe = nullptr;
    Player *player = nullptr;

    // Renderer
    Scene *scene = nullptr;
};