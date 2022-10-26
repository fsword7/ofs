// starlib.h - Star database library package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

class StarTree;
class celStar;
class ofsHandler;
class ofsHandler2;

class StarDatabase
{
public:
    StarDatabase() = default;
    ~StarDatabase() = default;

    bool loadXHIPData(const fs::path &pname);

    void initOctreeData(std::vector<celStar *> stars);
    void finish();

    void addStar(celStar *star);

    celStar *find(cstr_t &name) const;

    void findVisibleStars(const ofsHandler2 &handle, const glm::dvec3 &obs,
        const glm::dmat3 &rot, double fov, double aspect, double limitMag) const;
    int findCloseStars(const glm::dvec3 &obs, double radius,
        std::vector<ObjectHandle> &stars) const;

    inline celStar *getHIPstar(uint32_t hip) const          { return hipList[hip]; }
    inline ObjectHandle getHIPstar2(uint32_t hip) const     { return hipList[hip]; }

private:
    std::vector<celStar *> uStars;

    celStar **hipList = nullptr;

    StarTree *starTree = nullptr;
};