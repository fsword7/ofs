// handle.h - OFS handler package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#pragma once

class ofsHandler
{
public:
    ofsHandler() = default;
    ~ofsHandler() = default;

    virtual void process(ObjectHandle star, double dist, double appMag) const = 0;
};