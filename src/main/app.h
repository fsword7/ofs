// app.h - Core application package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class CoreApp
{
public:
    CoreApp() = default;
    ~CoreApp() = default;

protected:
    bool stateKey[512];
    bool shiftStateKey[512];
    bool ctrlStateKey[512];
    bool altStateKey[512];
    

};