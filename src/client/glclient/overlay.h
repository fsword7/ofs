// overlay.h - overlay text display package
//
// Author:  Tim Stark
// Date:    Oct 22, 2023

#pragma once

class Context;
class TextureFont;
class Overlay;

#include "fonts.h"

class OverlayBuffer : public std::streambuf
{
public:
    OverlayBuffer();

    void set(Overlay *obuf) { overylay = obuf; }

private:
    Overlay *overylay = nullptr;
};

class Overlay : public std::ostream
{
public:
    Overlay(Scene &scene);
    ~Overlay() = default;

    void setFont(TextureFont *font);
    void setPosition(float x, float y);
    void setColor(const color_t &color);

    void reset();
    void home();
    void moveBy(float x, float y);

    void print(cstr_t &text);

private:
    Scene &scene;

    OverlayBuffer obuf;

    float width, height;

    TextureFont *font = nullptr;
    bool fontChanged = false;
    color_t color;

    float xOff   = 0.0f;
    float yOff   = 0.0f;
    float ySpace = 3.0f;
};