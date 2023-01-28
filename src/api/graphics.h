// graphics.h - Graphics API package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#pragma once

#include "api/ofsapi.h"
#include <GLFW/glfw3.h>

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
    virtual void cbStart() = 0;
    virtual void cbRenderScene() = 0;

};

OFSAPI bool ofsRegisterGraphicsClient(GraphicsClient *gc);
OFSAPI bool ofsUnregisterGraphicsClient(GraphicsClient *gc);
