// engine.h - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class Scene;
class Player;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

    inline Scene *getScene()   { return scene; }
    inline Player *getPlayer() { return player; }
    
    void init(Context *ctx, int width, int height);
    void update(double dt);
    void render();

private:
    Player *player = nullptr;

    // Renderer
    Scene *scene = nullptr;
};