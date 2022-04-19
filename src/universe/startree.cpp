// startree.cpp - Star octree package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#include "main/core.h"
#include "engine/object.h"
#include "universe/star.h"
#include "universe/startree.h"
#include "universe/astro.h"

using namespace astro;

StarTree::StarTree(const vec3d_t &cell, const double factor, StarTree *parent)
: Tree(parent), cellCenter(cell), exclusiveFactor(factor)
{
    list.clear();
}

StarTree::~StarTree()
{
    deleteChildren();
}

double StarTree::decay(double factor)
{
    return convertLumToAbsMag(convertAbsMagToLum(factor) / 4.0);
}

void StarTree::add(const celStar &star)
{
    list.push_back(&star);
}

void StarTree::insert(const celStar &star, double scale)
{
    StarTree *child;

    if (star.getAbsMag() < exclusiveFactor)
        add(star);
    else if ((child = getChild(index(star, cellCenter))) != nullptr)
        child->insert(star, scale * 0.5);
    else
    {
        if (list.size() > STARTREE_THRESHOLD)
            split(scale * 0.5);
        add(star);
    }
}

void StarTree::split(double scale)
{
    int count = 0;

    for (int idx = 0; idx < list.size(); idx++)
    {
        const celStar &star = *list[idx];

        if (star.getAbsMag() < exclusiveFactor)
            list[count++] = list[idx];
        else
        {
            vec3d_t cell = cellCenter;
            uint32_t idx = index(star, cell);
            StarTree *node = getChild(idx);

            if (node == nullptr)
            {
                cell += vec3d_t(((idx & xPos) != 0) ? scale : -scale,
                                ((idx & yPos) != 0) ? scale : -scale,
                                ((idx & zPos) != 0) ? scale : -scale);
                
                node = new StarTree(cell, decay(exclusiveFactor), this);
                addChild(idx, node);
            }

            node->add(star);
        }
    }

    list.resize(count);
}

uint32_t StarTree::index(const celStar &star, const vec3d_t &cell)
{
    vec3d_t spos = star.getStarPosition();

    return ((spos.x < cell.x) ? 0 : xPos) |
           ((spos.y < cell.y) ? 0 : yPos) |
           ((spos.z < cell.z) ? 0 : zPos);
}

uint32_t StarTree::countNodes()
{
    StarTree *child;
    uint32_t count = 1;

    for (int idx = 0; idx < 8; idx++)
        if ((child = getChild(idx)) != nullptr)
            count += child->countNodes();

    return count;
}

uint32_t StarTree::countObjects()
{
    StarTree *child;
    uint32_t count = list.size();

    for (int idx = 0; idx < 8; idx++)
        if ((child = getChild(idx)) != nullptr)
            count += child->countObjects();

    return count;
}

void StarTree::processVisibleStars(const ofsHandler &handle, const vec3d_t &obs,
    const double limitingFactor, const double scale)
{
    double dist = glm::length(obs - cellCenter) - scale * sqrt(3.0);

    for (int32_t idx = 0; idx < list.size(); idx++)
    {
        const celStar &star = *list[idx];

        double dist = glm::length(obs - star.getStarPosition());
        double appMag = convertAbsToAppMag(star.getAbsMag(), dist);

        handle.process(star, dist, appMag);
    }

    if (dist <= 0 || convertAbsToAppMag(exclusiveFactor, dist) <= limitingFactor)
    {
        for (int idx = 0; idx < 8; idx++)
        {
            StarTree *node = getChild(idx);
            if (node == nullptr)
                continue;
            node->processVisibleStars(handle, obs, limitingFactor, scale * 0.5);
        }
    }
}

void StarTree::processCloseStars(const vec3d_t &obs, const double radius, const double scale,
    std::vector<const celStar *> &stars)
{
    double dist = glm::length(obs - cellCenter) - scale * sqrt(3.0);
    if (dist > radius)
        return;
    
    for (uint32_t idx = 0; idx < list.size(); idx++)
    {
        const celStar *star = list[idx];

        if (glm::length(obs - star->getStarPosition()) < square(radius))
        {
            double dist = glm::length(obs - star->getStarPosition());
            if (dist < radius)
                stars.push_back(star);
        }
    }

    for (int idx = 0; idx < 8; idx++)
    {
        StarTree *node = getChild(idx);
        if (node == nullptr)
            continue;
        node->processCloseStars(obs, radius, scale * 0.5, stars);
    }
}