// frame.cpp - Frame Reference package
//
// Author:  Tim Stark
// Date:    Apt 21, 2022

#include "main/core.h"
#include "universe/frame.h"

// ******** Reference Frame Tree ********

FrameTree::FrameTree(celStar *star)
: parentStar(star)
{
}

FrameTree::FrameTree(celBody *body)
: parentBody(body)
{
}

FrameTree::~FrameTree()
{
}

void FrameTree::addObject(Object *object)
{
    objects.push_back(object);
}

Object *FrameTree::getObject(int idx) const
{
    if (idx >= 0 && idx < objects.size())
        return objects[idx];
    return nullptr;
}

// ******** Reference Frame ********

