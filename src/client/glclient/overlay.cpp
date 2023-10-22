// overlay.cpp - overlay text display package
//
// Author:  Tim Stark
// Date:    Oct 22, 2023

#include "main/core.h"
#include "client.h"
#include "shader.h"
#include "fonts.h"
#include "scene.h"
#include "overlay.h"

OverlayBuffer::OverlayBuffer()
{
    setbuf(nullptr, 0);
}

Overlay::Overlay(Scene &scene)
: std::ostream(&obuf), scene(scene)
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
    width  = scene.getWidth();
    height = scene.getHeight();

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
