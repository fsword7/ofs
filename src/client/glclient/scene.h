// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

#include "shader.h"

class Camera;
class vObject;
class Scene
{
public:
    Scene(int width, int height);
    ~Scene() = default;

    inline ShaderManager &getShaderManager()        { return shmgr; }
    inline Camera *getCamera(int idx = 0) const     { return camera; }

    void init();
    void start();
    void update();
    void render();

    vObject *addVisualObject(ObjectHandle object);
    vObject *getVisualObject(ObjectHandle object, bool bCreate = false);

private:
    int width, height;

    ShaderManager shmgr;

    Camera *camera = nullptr;

    std::vector<vObject *> vobjList;

    vObject *vEarth = nullptr;
};