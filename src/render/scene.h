// scene.h - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Universe;
class Player;

class Scene
{
public:
    Scene(Context &gl) : ctx(gl)
    { }
    ~Scene() = default;

    void init();
    void render(Player &player);

private:
    Context &ctx;
};