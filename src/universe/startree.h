// startree.h - Star octree package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

#include "utils/tree.h"
#include "api/handle.h"
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

    StarTree(const glm::dvec3 &cell, const double factor, StarTree *parent = nullptr);
    ~StarTree();

    void insert(celStar &object, double scale);
    void add(celStar &object);
    void split(double scale);

    uint32_t countNodes();
    uint32_t countObjects();

    void processVisibleStars(const ofsHandler2 &handle, const glm::dvec3 &obs,
        const double limitingFactor, const double scale);
    void processCloseStars(const glm::dvec3 &obs, const double radius, const double scale,
        std::vector<ObjectHandle> &stars);

private:
    double decay(double factor);
    uint32_t index(celStar &object, const glm::dvec3 &cell);

    glm::dvec3 cellCenter;
    double  exclusiveFactor;

    std::vector<celStar *> list;
};