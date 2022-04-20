// starlib.h - Star database library package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

class StarTree;
class celStar;
class ofsHandler;

class StarDatabase
{
public:
    StarDatabase() = default;
    ~StarDatabase() = default;

    bool loadXHIPData(const fs::path &pname);

    void initOctreeData(std::vector<celStar *> stars);
    void finish();

    void addStar(celStar *star);

    celStar *find(cstr_t &name);

    void findVisibleStars(const ofsHandler &handle, const vec3d_t &obs,
        const quatd_t &rot, double fov, double aspect, double limitMag) const;
    int findCloseStars(const vec3d_t &obs, double radius,
        std::vector<const celStar *> &stars) const;

private:
    std::vector<celStar *> uStars;

    celStar **hipList = nullptr;

    StarTree *starTree = nullptr;
};