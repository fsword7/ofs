// app.h - Core application package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Context;

class CoreApp
{
public:
    CoreApp();
    ~CoreApp() = default;

    // Virtual main function calls packages
    virtual void init() = 0;
    virtual void cleanup() = 0;
    virtual void run() = 0;

protected:
    Context *ctx = nullptr;
    int width, height;

    bool stateKey[512];
    bool shiftStateKey[512];
    bool ctrlStateKey[512];
    bool altStateKey[512];
};