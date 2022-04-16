// app.h - Core application package for SDL2 interface
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/app.h"

class sdlCoreApp : public CoreApp
{
public:
    sdlCoreApp() = default;
    ~sdlCoreApp() = default;

    void init() override;
    void cleanup() override;
    void run() override;

private:
    SDL_Window *window = nullptr;
};