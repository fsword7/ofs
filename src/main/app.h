// app.h - Core application package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Context;
class Engine;
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
    Context *ctx = nullptr;
    Scene *scene = nullptr;
    Engine *engine = nullptr;
    int width, height;

    bool stateKey[512];
    bool shiftStateKey[512];
    bool ctrlStateKey[512];
    bool altStateKey[512];
};