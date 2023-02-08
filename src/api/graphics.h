// graphics.h - Graphics API package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#pragma once

#include "api/ofsapi.h"
#include <GLFW/glfw3.h>

class Universe;
class Player;

class OFSAPI GraphicsClient
{
public:
    GraphicsClient(ModuleHandle handle);
    virtual ~GraphicsClient();

    virtual bool cbInitialize() { return false; }
    virtual void cbCleanup() { }

    virtual GLFWwindow *cbCreateRenderingWindow() = 0;
    virtual bool cbDisplayFrame() = 0;
    virtual void cbSetWindowTitle(cstr_t &title) = 0;
    virtual void cbStart(Universe *universe) = 0;
    virtual void cbRenderScene(Player *player) = 0;

    virtual void setViewportSize(int width, int height) = 0;
    virtual void hideWindow() = 0;
    virtual void showWindow() = 0;

    virtual void startImGuiNewFrame() = 0;
    virtual void renderImGuiDrawData() = 0;
};

OFSAPI bool ofsRegisterGraphicsClient(GraphicsClient *gc);
OFSAPI bool ofsUnregisterGraphicsClient(GraphicsClient *gc);
OFSAPI void ofsInitGLFW(GLFWwindow *window);
