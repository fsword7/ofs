// draw.h - Sketchpad 2D drawing API package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#pragma once

#include "api/ofsapi.h"

typedef void * SurfaceHandle;

class DrawingTool
{

};

class OFSAPI Pen
{
public:
    Pen(color_t color, int style, int width) { }
    virtual ~Pen() = default;
};

class OFSAPI Sketchpad
{
public:
    Sketchpad(SurfaceHandle *handle = nullptr) : surf(handle) { }
    virtual ~Sketchpad() = default;

    inline SurfaceHandle getSurface() const     { return surf; }

    virtual Pen *setPen(Pen *pen)               { return nullptr; }
    virtual color_t setTextColor(color_t col)   { return color_t(); }

    // text function calls
    virtual bool text(int x, int y, const char *str, int len) { return false; }
    virtual bool textw(int x, int y, const wchar_t *str, int len) { return false; }
    virtual bool text(int x, int y, const std::string &str) { return false; }

    // draw function calls
    virtual void moveTo(int x, int y) { }
    virtual void lineTo(int x, int y) { }
    virtual void line(int x0, int y0, int x1, int y1) { }
    virtual void rectangle(int x0, int y0, int x1, int y1) { }
    virtual void ellipse(int x0, int y0, int x1, int y1) { }

private:
    SurfaceHandle surf = nullptr;
};