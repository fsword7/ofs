// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

class vObject;
class Scene
{
public:
    Scene(int width, int height);
    ~Scene() = default;

    void init();
    void start();
    void render();

    vObject *addVisualObject(ObjectHandle object);
    vObject *getVisualObject(ObjectHandle object, bool bCreate = false);

private:
    int width, height;

    std::vector<vObject *> vobjList;

    vObject *vEarth = nullptr;
};