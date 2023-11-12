// skpad.h - Sketchpad 2D Vector drawing tool package for MFD/HUD panels
//
// Author:  Tim Stark
// Date:    Nov 8, 2023

#pragma once

#include "api/draw.h"

class NVGcontext;
class NVGcolor;
class glFont : public Font
{
    friend class glPad;

public:
    glFont(int height, bool fixed, cchar_t *face, Style style = Normal, float orientation = 0.0f, bool antialiased = true);
    ~glFont() = default;

private:
    std::string faceName;
    std::string fontFile;
    int   fontHeight = 0;
    float rotRadians = 0.0f;
    bool  bAntialiased = true;
};

class glPen : public Pen
{
    friend class glPad;

public:
    glPen(color_t color, int width, int style)
    : Pen(color, width, style),
      color(color), width(width), style(style)
    { }

private:
    color_t color;
    int width;
    int style;
};

class glBrush : public Brush
{
    friend class glPad;

public:
    glBrush(color_t color)
    : Brush(color), color(color)
    { }

private:
    color_t color;
};

class glPad : public Sketchpad
{
public:
    glPad(int width, int height, bool antialiased);
    virtual ~glPad();

    Font *setFont(Font *font) override;
    Pen *setPen(Pen *pen) override;
    Brush *setBrush(Brush *brush) override;
    color_t setTextColor(color_t color) override;
    color_t setBackgroundColor(color_t color) override;

    void begin();
    void end();

    void beginPath();
    void endPath();

    void setOrigin(int x, int y);
    void getOrigin(int &x, int &y);

    void moveTo(int x, int y) override;
    void lineTo(int x, int y) override;
    void line(int x0, int y0, int x1, int y1) override;

    bool text(int x, int y, cchar_t *str, int len) override;

private:
    NVGcontext *ctx = nullptr;
    int width;
    int height;

    mutable glFont *cFont = nullptr;
    mutable glPen *cPen = nullptr;
    mutable glBrush *cBrush = nullptr;

    color_t  textColor;
    color_t  bgColor;
    int      textAlign;
    // NVGcolor nvgTextColor;

    int xOrigin = 0;
    int yOrigin = 0;
    int xCurrent;
    int yCurrent;
};