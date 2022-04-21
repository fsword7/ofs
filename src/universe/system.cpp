// system.cpp - Solar System package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/system.h"

// ******** Solar System ********

System::System(celStar *star)
: objects(star)
{
    stars.push_back(star);
    star->setSolarSystem(this);
}

celBody *System::createBody(cstr_t &name, PlanetarySystem *system,
    celType type, cstr_t &orbitFrameName, cstr_t &bodyFrameName)
{
    celBody *body = new celBody(system, name, type);

    Object *parentObject = system->getPrimaryBody();
    if (parentObject == nullptr)
        parentObject = system->getStar();

    fmt::printf("Body %s -> %s\n", name, parentObject->getsName());

    return body;
}