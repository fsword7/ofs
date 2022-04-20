// maingl.cpp - Main SDL2 routines for OpenGL interface
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "main/sdl/app.h"

void sdlCoreApp::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fmt::printf("OFS: Unable to initialize SDL2 inteface: %s\n",
            SDL_GetError());
        abort();
    }

    ctx = new Context();
    window = ctx->createWindow(APP_SHORT, width, height);
    ctx->init(width, height);
}

void sdlCoreApp::cleanup()
{
    SDL_Quit();
}

void sdlCoreApp::run()
{
    bool running = true;

    start();
    while(running)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            
            }
        }

        update();
        render();
        SDL_GL_SwapWindow(window);
    }
}

int main(int argc, char **argv)
{
    std::cout << "Welcome to Orbital Flight Simulator (with OpenGL interface)" << std::endl;

    sdlCoreApp app;

    app.init();
    app.initEngine();

    app.run();
    app.cleanup();

    return 0;
}