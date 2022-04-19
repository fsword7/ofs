// starlib.h - Star database library package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

class celStar;

class StarDatabase
{
public:
    StarDatabase() = default;
    ~StarDatabase() = default;

    void load();

    celStar *find(cstr_t &name);
 
    int findCloseStars(const vec3d_t &obs, double radius,
        std::vector<const celStar *> &stars) const;

private:
    std::vector<celStar *> uStars;

    celStar **hipList = nullptr;
};