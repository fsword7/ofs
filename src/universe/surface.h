// surface.h - Celestial body surface package
//
// Author:  Tim Stark
// Date:    May 16, 2022

#pragma once

class celSurface
{
public:
    celSurface() = default;
    ~celSurface() = default;

    inline void setColor(color_t nColor)    { color = nColor; }

public:
    color_t color   = { 1.0f, 1.0f, 1.0f };
    color_t spColor = { 0.0f, 0.0f, 0.0f };
    float   spPower = 0.0f;
};