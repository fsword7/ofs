// handle.h - OFS handler package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#pragma once

class ofsHandler2
{
public:
    ofsHandler2() = default;
    ~ofsHandler2() = default;

    virtual void process(ObjectHandle star, double dist, double appMag) const = 0;
};