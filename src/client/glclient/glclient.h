// oglclient.h - OpenGL Graphics Client package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#pragma once

#include "api/module.h"
#include "api/graphics.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

class Scene;

class glClient : public GraphicsClient
{
public:
    glClient(ModuleHandle handle) : GraphicsClient(handle) {}
    virtual ~glClient() {}

    bool cbInitialize() override;

    bool cbCreateRenderingWindow() override;
    bool cbDisplayFrame() override;

    void cbRenderScene() override;

private:
    int width, height;

    SDL_Window *window = nullptr;
    SDL_GLContext ctx = nullptr;

    Scene *scene = nullptr;
};