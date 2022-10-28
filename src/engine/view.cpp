// view.cpp - controllable window view package
//
// Author:  Tim Stark
// Date:    Apr 28, 2022

#include "main/core.h"
// #include "engine/player.h"
// #include "render/scene.h"
#include "engine/view.h"

// Convert window X, Y to view X, Y [-1:1]
void View::map(float wx, float wy, float &vx, float &vy) const
{
    vx = (wx - x) / width;
    vy = (wy + (y + height - 1.0f)) / height;

    // Convert to [-1:1]
    vx = (vx - 0.5f) * (width / height);
    vy = 0.5f - vy;
}
