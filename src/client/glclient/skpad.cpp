// skpad.cpp - Sketchpad 2D Vector drawing tool package for MFD/HUD panels
//
// Author:  Tim Stark
// Date:    Nov 8, 2023

#include <fontconfig/fontconfig.h>

#include "main/core.h"
#include "client.h"
#include "scene.h"
#include "renderer.h"
#include "texmgr.h"
#include "skpad.h"

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"

static std::map<std::string, std::string> fontCache;

NVGcontext *ctxNormal = nullptr;
NVGcontext *ctxFlipped = nullptr;
NVGcontext *ctxNormalaa = nullptr;
NVGcontext *ctxFlippedaa = nullptr;

glFont::glFont(cchar_t *face, int height, bool fixed, Style style, float orientation, bool aa)
: Font(height, fixed, face, style, orientation),
  faceName(face), fontHeight(height),
  bAntialiased(aa), rotRadians(orientation)
{
    if (fixed == true)
        faceName = "Monospace";
    faceName += std::to_string(-height);

    if (style & Bold)
        faceName += ":bold";
    if (style & Italic)
        faceName += ":italic";
    if (style & Underline)
        faceName += ":underline";

    // check font cache first
    std::string fname = faceName;
 
    glLogger->debug("Font {}: Fontcache: {}\n", face, fname);

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
            glLogger->debug("Found: {}\n", (char *)file);
        }
    }

    // clean up
    FcPatternDestroy(font);
    FcPatternDestroy(pat);
    FcConfigDestroy(config);
}

glPad::glPad(Texture *tex, bool antialiased)
: Sketchpad(), antiAliased(antialiased)
{
    textAlign = (NVGalign)(NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    textBkgMode = TRANSPARENT;
    txPad = dynamic_cast<glTexture *>(tex);

    if (txPad != nullptr) {
        width = txPad->getWidth();
        height = txPad->getHeight();
        ctx = antiAliased ? ctxFlippedaa : ctxFlipped;
    } else {
        width = glScene->getWidth();
        height = glScene->getHeight();
        ctx = antiAliased ? ctxNormalaa : ctxNormal;
    }
}

glPad::~glPad()
{
}

void glPad::ginit()
{
    ctxNormal = nvgCreateGL3(NVG_STENCIL_STROKES);
    ctxFlipped = nvgCreateGL3(NVG_STENCIL_STROKES|NVG_YFLIP);
    ctxNormalaa = nvgCreateGL3(NVG_STENCIL_STROKES|NVG_ANTIALIAS);
    ctxFlippedaa = nvgCreateGL3(NVG_STENCIL_STROKES|NVG_ANTIALIAS|NVG_YFLIP);

    assert(ctxNormal != nullptr && ctxFlipped != nullptr && 
           ctxNormalaa != nullptr && ctxFlippedaa != nullptr);
}

void glPad::gexit()
{
    nvgDeleteGL3(ctxNormal);
    nvgDeleteGL3(ctxFlipped);
    nvgDeleteGL3(ctxNormalaa);
    nvgDeleteGL3(ctxFlippedaa);
}

NVGcolor glPad::getNVGColor(color_t color)
{
    return nvgRGBAf(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
}

void glPad::beginDraw()
{   
    if (txPad != nullptr)
    {
        glRenderer::pushRenderTarget(txPad);
    }
    else
    {
        // gl::pushFlag(GL_CULL_FACE, false);
        glDisable(GL_CULL_FACE);
    }

    glClear(GL_STENCIL_BUFFER_BIT);
    nvgBeginFrame(ctx, width, height, 1.0);
}

void glPad::endDraw()
{
    nvgEndFrame(ctx);
    if (txPad != nullptr)
    {
        glRenderer::popRenderTarget();
        txPad->bind();
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        // gl::popFlag();
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
    }
    glRenderer::sync();
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
    if (cPen == nullptr && pen != nullptr)
    {
        cPen = ret;
        return nullptr;
    }

    return ret;
}

Brush *glPad::setBrush(Brush *brush)
{
    glBrush *ret = cBrush;
    cBrush = dynamic_cast<glBrush *>(brush);
    if (cBrush == nullptr && brush != nullptr)
    {
        cBrush = ret;
        return nullptr;
    }

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


void glPad::endStrokeFromBrush()
{
    if (cBrush == nullptr)
        return;
    
    nvgFillColor(ctx, getNVGColor(cBrush->color));
    nvgFill(ctx);
}

void glPad::endStrokeFromPen()
{
    if (cPen == nullptr)
        return;

    nvgStrokeColor(ctx, getNVGColor(cPen->color));    
    nvgStrokeWidth(ctx, std::max(1, cPen->width));
    nvgStrokeDash(ctx, cPen->style == 2 ? 1 : 0);
    nvgStroke(ctx);
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

void glPad::moveTo(int x, int y)
{
    xCurrent = x;
    yCurrent = y;
}

void glPad::drawLineTo(int x, int y)
{
    if (cPen->style != 0) {
        nvgBeginPath(ctx);
        nvgMoveTo(ctx, xOrigin + xCurrent, yOrigin + yCurrent);
        nvgLineTo(ctx, xOrigin + x, yOrigin + y);
        endStrokeFromPen();
    }

    xCurrent = x;
    yCurrent = y;
}

void glPad::drawLine(int x0, int y0, int x1, int y1)
{
    nvgBeginPath(ctx);
    moveTo(x0, y0);
    drawLineTo(x1, y1);
    endStrokeFromPen();
}

void glPad::drawRectangle(int x0, int y0, int x1, int y1)
{
    nvgBeginPath(ctx);
    nvgRect(ctx, xOrigin+x0, yOrigin+y0, abs(x1-x0), abs(y1-y0));
    endStrokeFromBrush();
    endStrokeFromPen();
}

void glPad::drawEllipse(int x0, int y0, int x1, int y1)
{
    nvgBeginPath(ctx);
    int cx = (x0+x1)/2;
    int cy = (y0+y1)/2;
    nvgEllipse(ctx, xOrigin+cx, yOrigin+cy, abs(x1-x0)/2, abs(y1-y0)/2);
    endStrokeFromBrush();
    endStrokeFromPen();
}

void glPad::drawPolygon(const glm::dvec2 *vtx, int nvtx)
{
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xOrigin+vtx[0].x, yOrigin+vtx[0].y);
    for (int idx = 1; idx < nvtx; idx++)
        nvgLineTo(ctx, xOrigin+vtx[idx].x, yOrigin+vtx[idx].y);
    nvgClosePath(ctx);
    endStrokeFromBrush();
    endStrokeFromPen();
}

void glPad::drawPolygonLine(const glm::dvec2 *vtx, int nvtx)
{
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xOrigin+vtx[0].x, yOrigin+vtx[0].y);
    for (int idx = 1; idx < nvtx; idx++)
        nvgLineTo(ctx, xOrigin+vtx[idx].x, yOrigin+vtx[idx].y);
    endStrokeFromPen();
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
    nvgRotate(ctx, cFont->rotRadians);

    nvgFillColor(ctx, textNVGColor);
    nvgText(ctx, xOrigin+x, yOrigin+y, str, str+len);

    nvgResetTransform(ctx);

    return true;   
}

void glPad::setTextPos(int x, int y)
{
    xText = x;
    yText = y;
}

int glPad::print(cstr_t &str)
{
    if (cFont == nullptr)
        return 0;

    float ascender, descender, lineh, acw;
    nvgTextMetrics(ctx, &ascender, &descender, &lineh, &acw);
    uint32_t height = lineh - (lineh - ascender + descender) / 2.0;
    
    cchar_t *cstr = str.c_str();
    int len = str.size();

    nvgFillColor(ctx, textNVGColor);
    nvgText(ctx, xText, yText, cstr, cstr+len);
    yText += height;

    return height;
}
