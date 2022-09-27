// startree.h - Star octree package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

#include "utils/tree.h"
#include "universe/handle.h"

#define STARTREE_MAGNITUDE      6.0
#define STARTREE_ROOTSIZE       (10'000'000.0 / LY_PER_PARSEC)
#define STARTREE_THRESHOLD      75

class celStar;

class StarTree : public Tree<StarTree, OTREE_NODES>
{
public:
    enum
    {
        xPos = 1,
        yPos = 2,
        zPos = 4
    };

    StarTree(const vec3d_t &cell, const double factor, StarTree *parent = nullptr);
    ~StarTree();

    void insert(const celStar &object, double scale);
    void add(const celStar &object);
    void split(double scale);

    uint32_t countNodes();
    uint32_t countObjects();

    void processVisibleStars(const ofsHandler &handle, const vec3d_t &obs,
        const double limitingFactor, const double scale);
    void processCloseStars(const vec3d_t &obs, const double radius, const double scale,
        std::vector<const celStar *> &stars);

    void processVisibleStars2(const ofsHandler &handle, const glm::dvec3 &obs,
        const double limitingFactor, const double scale);
    void processCloseStars2(const glm::dvec3 &obs, const double radius, const double scale,
        std::vector<ObjectHandle *> &stars);

private:
    double decay(double factor);
    uint32_t index(const celStar &object, const vec3d_t &cell);

    vec3d_t cellCenter;
    glm::dvec3 cellCenter2;
    double  exclusiveFactor;

    std::vector<const celStar *> list;
};