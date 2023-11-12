// skpad.cpp - Sketchpad 2D Vector drawing tool package for MFD/HUD panels
//
// Author:  Tim Stark
// Date:    Nov 8, 2023

#include <fontconfig/fontconfig.h>
#include "nanovg/src/nanovg.h"

#include "main/core.h"
#include "client.h"
#include "skpad.h"

static std::map<std::string, std::string> fontCache;

glFont::glFont(int height, bool fixed, cchar_t *face, Style style, float orientation, bool aa)
: Font(height, fixed, face, style, orientation),
  faceName(face), fontHeight(height),
  bAntialiased(aa), rotRadians(orientation)
{
    if (fixed == true)
        faceName = "Monospace";
    if (style & Bold)
        faceName += ":bold";
    if (style & Italic)
        faceName += ":italic";
    if (style & Underline)
        faceName += ":underline";

    // check font cache first
    std::string fname = faceName;
    fname += height;
    if (auto fc = fontCache.find(fname); fc != fontCache.end())
    {
        fontFile = fc->second;
        return;
    }

    FcConfig *config = FcInitLoadConfigAndFonts();
    FcPattern *pat = FcNameParse((const FcChar8 *)faceName.c_str());
    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);
    faceName += height;

    FcResult result;
    FcPattern *font = FcFontMatch(config, pat, &result);
    if (font != nullptr)
    {
        FcChar8 *file = nullptr;

        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
        {
            fontFile = (char *)file;
            fontCache[faceName] = fontFile;
        }
    }

    // clean up
    FcPatternDestroy(font);
    FcPatternDestroy(pat);
    FcConfigDestroy(config);
}

glPad::glPad(int w, int h, bool antialiased)
: Sketchpad(), width(w), height(h)
{

}

glPad::~glPad()
{
}

void glPad::begin()
{

}

void glPad::end()
{
    nvgEndFrame(ctx);
}

Font *glPad::setFont(Font *font)
{
    Font *ret = cFont;
    cFont = dynamic_cast<glFont *>(font);

    if (nvgFindFont(ctx, cFont->faceName.c_str()) == -1)
        nvgCreateFont(ctx, cFont->faceName.c_str(), cFont->fontFile.c_str());
    nvgFontSize(ctx, cFont->fontHeight);
    nvgFontFace(ctx, cFont->faceName.c_str());
    nvgTextAlign(ctx, textAlign);

    return ret;
}

Pen *glPad::setPen(Pen *pen)
{
    Pen *ret = cPen;
    cPen = dynamic_cast<glPen *>(pen);
    return ret;
}

Brush *glPad::setBrush(Brush *brush)
{
    Brush *ret = cBrush;
    cBrush = dynamic_cast<glBrush *>(brush);
    return ret;
}

color_t glPad::setTextColor(color_t color)
{
    color_t ret = textColor;
    textColor = color;
    // nvgTextColor = nvgRGBAf(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
    return ret;
}

color_t glPad::setBackgroundColor(color_t color)
{
    color_t ret = bgColor;
    bgColor = color;
    // nvgTextColor = nvgRGBAf(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
    return ret;
}

void glPad::setOrigin(int x, int y)
{
    xOrigin = x;
    yOrigin = y;
}

void glPad::getOrigin(int &x, int &y)
{
    x = xOrigin;
    y = yOrigin;
}

void glPad::beginPath()
{
    nvgBeginPath(ctx);
}

void glPad::endPath()
{
    nvgClosePath(ctx);
}

void glPad::moveTo(int x, int y)
{
    nvgMoveTo(ctx, xOrigin + x, yOrigin + y);
    xCurrent = x;
    yCurrent = y;
}

void glPad::lineTo(int x, int y)
{
    nvgLineTo(ctx, xOrigin + x, yOrigin + y);
    xCurrent = x;
    yCurrent = y;
}

void glPad::line(int x0, int y0, int x1, int y1)
{
    moveTo(x0, y0);
    lineTo(x1, y1);
}

bool glPad::text(int x, int y, cchar_t *str, int len)
{
    nvgResetTransform(ctx);
    nvgTranslate(ctx, xOrigin + x, yOrigin + y);
    nvgRotate(ctx, cFont->rotRadians);

    // nvgFillColor(ctx, nvgTextColor);
    nvgText(ctx, 0, 0, str, str+len);

    nvgResetTransform(ctx);

    return true;   
}