// oglclient.h - OpenGL Graphics Client package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#pragma once

#include "glad/gl.h"

#include "api/logger.h"
#include "api/module.h"
#include "api/graphics.h"
#include "api/draw.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

class Universe;
class Player;
class Scene;
class TextureManager;
struct Texture;

class glClient : public GraphicsClient
{
public:
    glClient(ModuleHandle handle) : GraphicsClient(handle) {}
    virtual ~glClient() {}

    bool cbInitialize() override;
    void cbCleanup() override;

    inline Sketchpad *getSketchpad() const override { return skpad; }

    GLFWwindow *cbCreateRenderingWindow() override;
    bool cbDisplayFrame() override;
    void cbSetWindowTitle(cstr_t &title) override;

    void loadTextureFont();

    Texture *createSurface(int width, int height, uint32_t flags = 0);
    void releaseSurface(Texture *surf);
    void clearSurface(Texture *surf, const color_t &color) override;

    void cbStart(Universe *universe) override;
    void cbRenderScene(Player *player) override;

    void setViewportSize(int width, int height) override;
    void showWindow() override;
    void hideWindow() override;

    void startImGuiNewFrame() override;
    void renderImGuiDrawData() override;

    double getElevationData(CelestialBody *cbody, glm::dvec3 loc, int reqlod = 0,
        elevTileList_t *tiles = nullptr, glm::dvec3 *normal = nullptr, int *lod = 0) override;

    Font *createFont(int height, bool fixed, cchar_t *face, Font::Style style, int orientation, bool antialiased) override;
    Pen *createPen(color_t color, int width, int style) override;
    Brush *createBrush(color_t color) override;

    void releaseFont(Font *font);
    void releasePen(Pen *pen);
    void releaseBrush(Brush *brush);

private:
    int width, height;

    GLFWwindow *window = nullptr;
    Sketchpad *skpad = nullptr;
    TextureManager *texmgr = nullptr;

    Scene *scene = nullptr;
};

extern ofsLogger *glLogger;

inline void checkErrors()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        glLogger->debug("OpenGL Error: {}\n", err);
    }
}