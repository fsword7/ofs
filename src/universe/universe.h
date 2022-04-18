// universe.h - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

class celStar;

class Universe
{
public:
    Universe() = default;
    ~Universe() = default;

    void init();

    celStar *findStar(cstr_t &name);

private:
    celStar *sun = nullptr;
};