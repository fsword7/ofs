// maingl.cpp - Main SDL2 routines for OpenGL interface
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "engine/player.h"
#include "engine/view.h"
#include "main/sdl/app.h"

#include <fmt/printf.h>

void sdlCoreApp::init()
{
    Logger::create(Logger::logDebug, "ofs.log");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::getLogger()->fatal("OFS: Unable to initialize SDL2 interface: {}\n",
            SDL_GetError());
        // fmt::printf("OFS: Unable to initialize SDL2 inteface: %s\n",
        //     SDL_GetError());
        abort();
    }

    ctx = new Context();
    window = ctx->createWindow(APP_SHORT, width, height);
    ctx->init(width, height);
}

void sdlCoreApp::cleanup()
{
    SDL_Quit();
    Logger::destroyLogger();
}

void sdlCoreApp::processKeyEvent(SDL_KeyboardEvent *key, bool down)
{
    keyCode  code = keyUndefined;
    char32_t ch;
    uint16_t mod = 0;

    if (key->keysym.mod & KMOD_SHIFT)
        mod |= keyShift;
    if (key->keysym.mod & KMOD_CTRL)
        mod |= keyControl;
    if (key->keysym.mod & KMOD_ALT)
        mod |= keyAlt;

    switch (key->keysym.sym)
    {
    case SDLK_F1:       code = keyF1;       break;
    case SDLK_F2:       code = keyF2;       break;
    case SDLK_F3:       code = keyF3;       break;
    case SDLK_F4:       code = keyF4;       break;
    case SDLK_F5:       code = keyF5;       break;
    case SDLK_F6:       code = keyF6;       break;
    case SDLK_F7:       code = keyF7;       break;
    case SDLK_F8:       code = keyF8;       break;
    case SDLK_F9:       code = keyF9;       break;
    case SDLK_F10:      code = keyF10;      break;
    case SDLK_F11:      code = keyF11;      break;
    case SDLK_F12:      code = keyF12;      break;

    case SDLK_KP_0:     code = keyPad0;     break;
    case SDLK_KP_1:     code = keyPad1;     break;
    case SDLK_KP_2:     code = keyPad2;     break;
    case SDLK_KP_3:     code = keyPad3;     break;
    case SDLK_KP_4:     code = keyPad4;     break;
    case SDLK_KP_5:     code = keyPad5;     break;
    case SDLK_KP_6:     code = keyPad6;     break;
    case SDLK_KP_7:     code = keyPad7;     break;
    case SDLK_KP_8:     code = keyPad8;     break;
    case SDLK_KP_9:     code = keyPad9;     break;

    case SDLK_UP:       code = keyUp;       break;
    case SDLK_DOWN:     code = keyDown;     break;
    case SDLK_LEFT:     code = keyLeft;     break;
    case SDLK_RIGHT:    code = keyRight;    break;

    case SDLK_HOME:     code = keyHome;     break;
    case SDLK_END:      code = keyEnd;      break;

    case SDLK_b:        code = keyb;        break;

    default:
        ch = key->keysym.sym;
        if ((down == false) || (ch & ~0xFF))
            break;
        keyEntered(ch, mod);
        break;
    }

    if (code > 0)
        keyPress(code, mod, down);
}

void sdlCoreApp::run()
{
    bool running = true;
    uint16_t mod;
    int   state;
    int   mx, my;
    float vx = 0.0f, vy = 0.0f;
    vec3d_t pickRay;

    std::string title;

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

            // Handling keyboard events
            case SDL_KEYDOWN:
                mod = event.key.keysym.mod;
                processKeyEvent(&event.key, true);
                break;

            case SDL_KEYUP:
                mod = event.key.keysym.mod;
                processKeyEvent(&event.key, false);
                break;

            // Handling joystick events

            // Handling mouse events
            case SDL_MOUSEMOTION:
                mx = event.motion.x;
                my = event.motion.y;

                state = 0;
                if (event.motion.state & SDL_BUTTON_LMASK)
                    state |= mouseLeftButton;
                if (event.motion.state & SDL_BUTTON_MMASK)
                    state |= mouseMiddleButton;
                if (event.motion.state & SDL_BUTTON_RMASK)
                    state |= mouseRightButton;
                if (mod & KMOD_CTRL)
                    state |= mouseControlButton;
                if (mod & KMOD_ALT)
                    state |= mouseAltButton;
                if (mod & KMOD_SHIFT)
                    state |= mouseShiftButton;

                if (activeView != nullptr)
                {
                    activeView->map(mx / float(width), my / float(height), vx, vy);
                    pickRay = player->getPickRay(vx, vy);
                }

                title = fmt::sprintf("%s X: %d Y %d (%f,%f) State: %c%c%c%c%c%c\n",
                    APP_SHORT, mx, my, pickRay.x, pickRay.y,
                    (state & mouseLeftButton    ? 'L' : '-'),
                    (state & mouseMiddleButton  ? 'M' : '-'),
                    (state & mouseRightButton   ? 'R' : '-'),
                    (state & mouseControlButton ? 'C' : '-'),
                    (state & mouseAltButton     ? 'A' : '-'),
                    (state & mouseShiftButton   ? 'S' : '-'));
                ctx->setWindowTitle(title);

                mouseMove(mx, my, state);
                break;

            case SDL_MOUSEBUTTONDOWN:
                mx = event.motion.x;
                my = event.motion.y;

                state = 0;
                if (event.motion.state & SDL_BUTTON_LMASK)
                    state |= mouseLeftButton;
                if (event.motion.state & SDL_BUTTON_MMASK)
                    state |= mouseMiddleButton;
                if (event.motion.state & SDL_BUTTON_RMASK)
                    state |= mouseRightButton;
                if (mod & KMOD_CTRL)
                    state |= mouseControlButton;
                if (mod & KMOD_ALT)
                    state |= mouseAltButton;
                if (mod & KMOD_SHIFT)
                    state |= mouseShiftButton;

                mousePressButtonDown(mx, my, state);
                break;

            case SDL_MOUSEBUTTONUP:
                mx = event.motion.x;
                my = event.motion.y;

                state = 0;
                if (event.motion.state & SDL_BUTTON_LMASK)
                    state |= mouseLeftButton;
                if (event.motion.state & SDL_BUTTON_MMASK)
                    state |= mouseMiddleButton;
                if (event.motion.state & SDL_BUTTON_RMASK)
                    state |= mouseRightButton;
                if (mod & KMOD_CTRL)
                    state |= mouseControlButton;
                if (mod & KMOD_ALT)
                    state |= mouseAltButton;
                if (mod & KMOD_SHIFT)
                    state |= mouseShiftButton;

                mousePressButtonUp(mx, my, state);
                break;

            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0)      // scroll up
                    mouseDialWheel(-1.0f, 0);
                else if (event.wheel.y < 0) // scroll down
                    mouseDialWheel(1.0f, 0);
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