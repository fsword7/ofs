// app.h - Core application package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Context;
class Engine;
class Universe;
class Player;
class Camera;
class Scene;

class CoreApp
{
public:
    CoreApp();
    ~CoreApp() = default;

    // Virtual main function calls packages
    virtual void init() = 0;
    virtual void cleanup() = 0;
    virtual void run() = 0;

    void initEngine();

    void update();
    void render();

protected:
    Engine   *engine = nullptr;
    Universe *universe = nullptr;
    Player   *player = nullptr;
    Camera   *camera = nullptr;

    Context  *ctx = nullptr;
    Scene    *scene = nullptr;

    int width, height;

    bool stateKey[512];
    bool shiftStateKey[512];
    bool ctrlStateKey[512];
    bool altStateKey[512];
};