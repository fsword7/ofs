// engine.h - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class Scene;

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

    inline Scene *getScene() { return scene; }
    
    void init(Context *ctx, int width, int height);
    void update(double dt);
    void render();

private:
    // Renderer
    Scene *scene = nullptr;
};