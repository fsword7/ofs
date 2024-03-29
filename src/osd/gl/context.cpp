// context.cpp - OpenGL Context
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/shader.h"

SDL_Window *Context::createWindow(cchar_t *appName, int width, int height)
{
    window = SDL_CreateWindow(appName,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        Logger::getLogger()->fatal("SDL2 Window can't be created: {}\n", SDL_GetError());
        abort();
    }
    SDL_ShowWindow(window);

    // Select OpenGL version before starting up
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    ctx = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    return window;
}

void Context::destroyWindow()
{
    SDL_DestroyWindow(window);
    window = nullptr;
}

void Context::setWindowTitle(cstr_t &title)
{
    SDL_SetWindowTitle(window, title.c_str());
}

void Context::init(int w, int h)
{
    // Initialize GLEW package
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        Logger::getLogger()->fatal("GLEW error: {}\n",
            (char *)glewGetErrorString(err));
        abort();
    }

    Logger::getLogger()->info("Using GLEW version: {}\n", (char *)glewGetString(GLEW_VERSION));
    Logger::getLogger()->info("    OpenGL version: {}\n", (char *)glGetString(GL_VERSION));
    Logger::getLogger()->info("Using Eigen version {}.{}\n",
        EIGEN_MAJOR_VERSION, EIGEN_MINOR_VERSION);

    resize(w, h);

    smgr = new ShaderManager(*this);
}

void Context::resize(int w, int h)
{
    width = w;
    height = h;
    aspect = double(width) / double(height);

    // Update OpenGL coordinates
    glViewport(0, 0, width, height);

    Logger::getLogger()->info("Resize: width {} height {} aspect {}\n",
        width, height, aspect);
}

void Context::enableBlend()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void Context::disableBlend()
{
    glDisable(GL_BLEND);
}

void Context::start()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

void Context::finish()
{

}