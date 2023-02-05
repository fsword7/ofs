// oglclient.h - OpenGL Graphics Client package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#pragma once

#include "glad/gl.h"

#include "api/logger.h"
#include "api/module.h"
#include "api/graphics.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class Scene;

class glClient : public GraphicsClient
{
public:
    glClient(ModuleHandle handle) : GraphicsClient(handle) {}
    virtual ~glClient() {}

    bool cbInitialize() override;
    void cbCleanup() override;

    GLFWwindow *cbCreateRenderingWindow() override;
    bool cbDisplayFrame() override;
    void cbSetWindowTitle(cstr_t &title) override;

    void cbStart() override;
    void cbRenderScene() override;

    void setViewportSize(int width, int height) override;
    void showWindow() override;
    void hideWindow() override;

    void startImGuiNewFrame() override;
    void renderImGuiDrawData() override;

private:
    int width, height;

    GLFWwindow *window = nullptr;

    Scene *scene = nullptr;
};

extern ofsLogger *logger;