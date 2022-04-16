// context.h - OpenGL Context package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#include <GL/GLEW.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

class Context
{
public:
    Context() = default;
    ~Context() = default;
    
    SDL_Window *createWindow(cchar_t *appName, int width, int height);
    void setWindowTile(cstr_t &title);
    void destroyWindow();

    void init(int w, int h);
    void resize(int w, int h);

private:
    SDL_Window *window = nullptr;
    SDL_GLContext ctx = nullptr;

    int    width = 1;
    int    height = 1;
    double aspect = double(width) / double(height);
};