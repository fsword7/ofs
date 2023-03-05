// handle.h - OFS handler package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

class ofsHandler
{
public:
    ofsHandler() = default;
    ~ofsHandler() = default;

    virtual void process(CelestialStar &star, double dist, double appMag) const = 0;
};