// starlib.h - Star database library package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#pragma once

class StarTree;
class CelestialStar;
class ofsHandler;
class Object;
class OFSAPI StarDatabase
{
public:
    StarDatabase() = default;
    ~StarDatabase() = default;

    bool loadXHIPData(const fs::path &pname);

    void initOctreeData(std::vector<CelestialStar *> stars);
    void finish();

    void addStar(CelestialStar *star);

    CelestialStar *find(cstr_t &name) const;

    void findVisibleStars(const ofsHandler &handle, const glm::dvec3 &obs,
        const glm::dmat3 &rot, double fov, double aspect, double limitMag) const;
    int findCloseStars(const glm::dvec3 &obs, double radius,
        std::vector<const CelestialStar *> &stars) const;

    inline CelestialStar *getHIPstar(uint32_t hip) const    { return hipList[hip]; }
    // inline ObjectHandle getHIPstar2(uint32_t hip) const     { return hipList[hip]; }

private:
    std::vector<CelestialStar *> uStars;

    CelestialStar **hipList = nullptr;

    StarTree *starTree = nullptr;
};