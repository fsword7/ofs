// draw.h - Sketchpad 2D drawing API package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#pragma once

#include "api/ofsapi.h"

typedef void * SurfaceHandle;

class DrawingTool
{
public:
    DrawingTool() = default;
    virtual ~DrawingTool() = default;
};

class OFSAPI Font : public DrawingTool
{
public:
    enum Style
    {
        Normal = 0,
        Bold = 1,
        Italic = 2,
        Underline = 4
    };

protected:
    Font(int height, bool fixed, cchar_t *face, Style style=Normal, float orientation = 0.0f)
    { }

public:
    virtual ~Font() = default;
};

class OFSAPI Pen : public DrawingTool
{
protected:
    Pen(color_t color, int width, int style)
    { }

public:
    virtual ~Pen() = default;
};

class OFSAPI Brush : public DrawingTool
{
protected:
    Brush(color_t color)
    { }

public:
    virtual ~Brush() = default;
};

class OFSAPI Sketchpad
{
public:
    enum TAHorizontal { LEFT, CENTER, RIGHT };
    enum TAVertical { TOP, MIDDLE, BOTTOM, BASELINE };
    enum BkgMode { TRANSPARENT, OPAQUE };
    
    Sketchpad(SurfaceHandle *handle = nullptr) : surf(handle) { }
    virtual ~Sketchpad() = default;

    inline SurfaceHandle getSurface() const             { return surf; }

    virtual Font *setFont(Font *font)                   { return nullptr; }
    virtual Pen *setPen(Pen *pen)                       { return nullptr; }
    virtual Brush *setBrush(Brush *brush)               { return nullptr; }
    virtual color_t setTextColor(color_t col)           { return color_t(); }
    virtual color_t setBackgroundColor(color_t col)     { return color_t(); }

    virtual void beginDraw() { }
    virtual void endDraw() { }

    // text function calls
    virtual int getCharSize() { return -1; };
    virtual int getTextWidth(cchar_t *str, int len = 0) { return -1; };
    virtual void setTextAlign(TAHorizontal tah=LEFT, TAVertical=TOP) {}
    virtual void setTextBackgroundMode(BkgMode mode) {}
    virtual bool text(int x, int y, const char *str, int len = 0) { return false; }
    virtual bool textw(int x, int y, const wchar_t *str, int len = 0) { return false; }
    virtual bool textu16(int x, int y, const char16_t *str, int len = 0) { return false; }
    virtual bool textu32(int x, int y, const char32_t *str, int len = 0) { return false; }
    virtual bool text(int x, int y, const std::string &str) { return false; }

    // draw function calls
    virtual void moveTo(int x, int y) { }
    virtual void drawLineTo(int x, int y) { }
    virtual void drawLine(int x0, int y0, int x1, int y1) { }
    virtual void drawRectangle(int x0, int y0, int x1, int y1) { }
    virtual void drawEllipse(int x0, int y0, int x1, int y1) { }
    virtual void drawPolygon(const glm::dvec2 *vtx, int nvtx) { }
    virtual void drawPolygonLine(const glm::dvec2 *vtx, int nvtx) { }

private:
    SurfaceHandle surf = nullptr;
};