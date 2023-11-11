// oglclient.h - OpenGL Graphics Client package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#pragma once

#include "glad/gl.h"

#include "api/logger.h"
#include "api/module.h"
#include "api/graphics.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

class Universe;
class Player;
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

    void loadTextureFont();

    void cbStart(Universe *universe) override;
    void cbRenderScene(Player *player) override;

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

inline void checkErrors()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        logger->debug("OpenGL Error: {}\n", err);
    }
}