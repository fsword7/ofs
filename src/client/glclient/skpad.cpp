// skpad.cpp - Sketchpad 2D Vector drawing tool package for MFD/HUD panels
//
// Author:  Tim Stark
// Date:    Nov 8, 2023

#include <fontconfig/fontconfig.h>

#include "main/core.h"
#include "client.h"
#include "texmgr.h"
#include "skpad.h"

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"

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

glPad::glPad(Texture *tex, bool aa)
: Sketchpad(), txPad(tex), antiAliased(aa)
{
    textAlign = (NVGalign)(NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    textBkgMode = TRANSPARENT;
}

glPad::~glPad()
{
}

void glPad::ginit()
{

}

void glPad::gexit()
{

}

NVGcolor glPad::getNVGColor(color_t color)
{
    return nvgRGBAf(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
}

void glPad::begin()
{   
    if (txPad != nullptr)
    {
        width = txPad->txWidth;
        height = txPad->txHeight;
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    glClear(GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(ctx, width, height, 1.0);
}

void glPad::end()
{
    nvgEndFrame(ctx);
    if (txPad != nullptr)
    {
        txPad->bind();
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {

    }
}

Font *glPad::setFont(Font *font)
{
    glFont *ret = cFont;
    cFont = dynamic_cast<glFont *>(font);
    if (cFont == nullptr)
    {
        cFont = ret;
        return nullptr;
    }

    if (nvgFindFont(ctx, cFont->faceName.c_str()) == -1)
        nvgCreateFont(ctx, cFont->faceName.c_str(), cFont->fontFile.c_str(), antiAliased);
    nvgFontSize(ctx, cFont->fontHeight);
    nvgFontFace(ctx, cFont->faceName.c_str());
    nvgTextAlign(ctx, textAlign);

    return ret;
}

Pen *glPad::setPen(Pen *pen)
{
    glPen *ret = cPen;
    cPen = dynamic_cast<glPen *>(pen);
    if (cPen == nullptr)
    {
        cPen = ret;
        return nullptr;
    }

    nvgStrokeColor(ctx, getNVGColor(cPen->color));    
    nvgStrokeWidth(ctx, std::max(1, cPen->width));
    nvgStrokeDash(ctx, cPen->style == 2 ? 1 : 0);
    nvgStroke(ctx);

    return ret;
}

Brush *glPad::setBrush(Brush *brush)
{
    glBrush *ret = cBrush;
    cBrush = dynamic_cast<glBrush *>(brush);
    if (cBrush == nullptr)
    {
        cBrush = ret;
        return nullptr;
    }

    nvgFillColor(ctx, getNVGColor(cPen->color));
    nvgFill(ctx);

    return ret;
}

color_t glPad::setTextColor(color_t color)
{
    color_t ret = textColor;
    textColor = color;
    textNVGColor = getNVGColor(color);
    return ret;
}

color_t glPad::setBackgroundColor(color_t color)
{
    color_t ret = bgColor;
    bgColor = color;
    bgNVGColor = getNVGColor(color);
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


NVGalign glPad::toNVGTextAlign(TAHorizontal tah)
{
    switch (tah)
    {
    default:
    case LEFT:
        return NVG_ALIGN_LEFT;
    case CENTER:
        return NVG_ALIGN_CENTER;
    case RIGHT:
        return NVG_ALIGN_RIGHT;
    }
}

NVGalign glPad::toNVGTextAlign(TAVertical tav)
{
    switch (tav)
    {
    default:
    case TOP:
        return NVG_ALIGN_TOP;
    case MIDDLE:
        return NVG_ALIGN_MIDDLE;
    case BOTTOM:
        return NVG_ALIGN_BOTTOM;
    case BASELINE:
        return NVG_ALIGN_BASELINE;
    }
}

void glPad::setTextAlign(TAHorizontal tah, TAVertical tav)
{
    textAlign = (NVGalign)(toNVGTextAlign(tah) | toNVGTextAlign(tav));
}

void glPad::setTextBackgroundMode(BkgMode mode)
{
    textBkgMode = mode;
}

int glPad::getCharSize()
{
    if (cFont == nullptr)
        return -1;
    
    float ascender, descender, lineh, acw;
    nvgTextMetrics(ctx, &ascender, &descender, &lineh, &acw);
    uint32_t height = lineh - (lineh - ascender + descender) / 2.0;

    return height | (int(acw) << 16);
}

int glPad::getTextWidth(cchar_t *str, int len)
{
    if (cFont == nullptr)
        return -1;
    if (len == 0)
        len = strlen(str);

    float bounds[4];
    nvgTextBounds(ctx, 0, 0, str, str+len, bounds);

    return bounds[2]-bounds[0];
}

bool glPad::text(int x, int y, cchar_t *str, int len)
{
    if (cFont == nullptr)
        return false;
    if (len == 0)
        len = strlen(str);

    nvgResetTransform(ctx);
    nvgTranslate(ctx, xOrigin + x, yOrigin + y);
    nvgRotate(ctx, cFont->rotRadians);

    nvgFillColor(ctx, textNVGColor);
    nvgText(ctx, 0, 0, str, str+len);

    nvgResetTransform(ctx);

    return true;   
}