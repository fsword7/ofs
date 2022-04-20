// context.h - OpenGL Context package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#include <GL/GLEW.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

class ShaderManager;

class Context
{
public:
    Context() = default;
    ~Context() = default;
    
    inline ShaderManager *getShaderManager() const { return smgr; }
    
    SDL_Window *createWindow(cchar_t *appName, int width, int height);
    void setWindowTile(cstr_t &title);
    void destroyWindow();

    void init(int w, int h);
    void resize(int w, int h);

    void enableBlend();
    void disableBlend();

    void start();
    void finish();

private:
    SDL_Window *window = nullptr;
    SDL_GLContext ctx = nullptr;

    ShaderManager *smgr = nullptr;

    int    width = 1;
    int    height = 1;
    double aspect = double(width) / double(height);
};