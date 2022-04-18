// universe.cpp - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/astro.h"

void Universe::init()
{
    sun = celStar::createTheSun();
}

celStar *Universe::findStar(cstr_t &name)
{
    return sun;
}