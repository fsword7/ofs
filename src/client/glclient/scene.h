// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

class Scene
{
public:
    Scene(int width, int height);
    ~Scene() = default;

    void render();

private:
    int width, height;
};