// surfmg.h - Surface Manager package
//
// Author:  Tim Stark
// Date:    Oct 18, 2023

#pragma once

class SurfaceManager
{
public:
    SurfaceManager() = default;
    ~SurfaceManager() = default;

    inline void setColor(color_t nColor)    { color = nColor; }

public:
    color_t color   = { 1.0f, 1.0f, 1.0f };
    color_t spColor = { 0.0f, 0.0f, 0.0f };
    float   spPower = 0.0f;
};