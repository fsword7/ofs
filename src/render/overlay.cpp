// overlay.cpp - overlay text display package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/shader.h"
#include "render/gl/fonts.h"
#include "render/scene.h"
#include "render/overlay.h"

OverlayBuffer::OverlayBuffer()
{
    setbuf(nullptr, 0);
}

Overlay::Overlay(Context &ctx)
: std::ostream(&obuf), ctx(ctx)
{
    obuf.set(this);
    reset();
}

void Overlay::setFont(TextureFont *nFont)
{
    if (font != nullptr)
        fontChanged = true;
    font = nFont;
}

void Overlay::setColor(const color_t &nColor)
{
    color = nColor;
}

void Overlay::setPosition(float x, float y)
{
    xOff = x;
    yOff = y;
}

void Overlay::reset()
{
    width  = ctx.getWidth();
    height = ctx.getHeight();

    home();
}

void Overlay::home()
{
    xOff = 0.0f;
    yOff = height;
}

void Overlay::moveBy(float x, float y)
{
    xOff += x;
    yOff -= y;
}

void Overlay::print(cstr_t &text)
{
    yOff -= font->getHeight() + ySpace;
    font->render(text, xOff, yOff, color);
}
