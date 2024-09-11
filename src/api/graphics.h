// graphics.h - Graphics API package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#pragma once

#include "api/ofsapi.h"
#include "api/draw.h"
#include "api/elevmgr.h"
#include <GLFW/glfw3.h>

class Universe;
class CelestialBody;
class Player;
class Sketchpad;
class Texture;
class Font;
class Pen;
class Brush;

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

    virtual Sketchpad *createSketchpad(Texture *tex, bool antialiased = false) { return nullptr; }
    virtual Sketchpad *getSketchpad() const = 0;
    virtual Texture *createSurface(int width, int height, uint32_t flags = 0) = 0;
    virtual void releaseSurface(Texture *surf) = 0;
    virtual void clearSurface(Texture *surf, const color_t &color) = 0;

    virtual void setViewportSize(int width, int height) = 0;
    virtual void hideWindow() = 0;
    virtual void showWindow() = 0;

    virtual void startImGuiNewFrame() = 0;
    virtual void renderImGuiDrawData() = 0;

    // virtual double getElevationData(CelestialBody *cbody, glm::dvec3 loc, int reqlod = 0,
    //     elevTileList_t *tiles = nullptr, glm::dvec3 *normal = nullptr, int *lod = 0) = 0;

    virtual Font *createFont(cchar_t *face, int height, bool fixed,
        Font::Style style = Font::Style::Normal, int orientation = 0, bool antialiased = false) = 0;
    virtual Pen *createPen(color_t color, int width, int style) = 0;
    virtual Brush *createBrush(color_t color) = 0;

    virtual void releaseFont(Font *font) = 0;
    virtual void releasePen(Pen *pen) = 0;
    virtual void releaseBrush(Brush *brush) = 0;
};

OFSAPI bool ofsRegisterGraphicsClient(GraphicsClient *gc);
OFSAPI bool ofsUnregisterGraphicsClient(GraphicsClient *gc);
OFSAPI void ofsInitGLFW(GLFWwindow *window);
