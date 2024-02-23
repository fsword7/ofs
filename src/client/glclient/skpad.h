// skpad.h - Sketchpad 2D Vector drawing tool package for MFD/HUD panels
//
// Author:  Tim Stark
// Date:    Nov 8, 2023

#pragma once

#include "nanovg/src/nanovg.h"
#include "api/draw.h"

class glFont : public Font
{
    friend class glPad;

public:
    glFont(cchar_t *face, int height, bool fixed, Style style = Normal, float orientation = 0.0f, bool antialiased = true);
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
    glPad(Texture *tex, bool antialiased);
    virtual ~glPad();

    static void ginit();
    static void gexit();

    NVGcolor getNVGColor(color_t color);
    
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
    void drawLineTo(int x, int y) override;
    void drawLine(int x0, int y0, int x1, int y1) override;
    void drawRectangle(int cx, int cy, int x1, int y1) override;
    void drawEllipse(int cx, int cy, int x1, int y1) override;
    void drawPolygon(const glm::dvec2 *vtx, int nvtx) override;
    void drawPolygonLine(const glm::dvec2 *vtx, int nvtx) override;

    int getCharSize() override;
    int getTextWidth(cchar_t *str, int len = 0) override;
    void setTextAlign(TAHorizontal tah=LEFT, TAVertical tav=TOP) override;
    void setTextBackgroundMode(BkgMode mode) override;
    bool text(int x, int y, cchar_t *str, int len = 0) override;
    
    void setTextPos(int x, int y) override;
    int print(cstr_t &str) override;

protected:
    NVGalign toNVGTextAlign(TAHorizontal tah);
    NVGalign toNVGTextAlign(TAVertical tav);

private:
    NVGcontext *ctx = nullptr;
    Texture *txPad = nullptr;
    int width;
    int height;

    mutable glFont *cFont = nullptr;
    mutable glPen *cPen = nullptr;
    mutable glBrush *cBrush = nullptr;

    BkgMode  textBkgMode;
    color_t  textColor;
    NVGcolor textNVGColor;
    color_t  bgColor;
    NVGcolor bgNVGColor;
    NVGalign textAlign;
    bool     antiAliased;

    int xOrigin = 0;
    int yOrigin = 0;
    int xCurrent;
    int yCurrent;
    int xText, yText;
};