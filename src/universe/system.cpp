// system.cpp - Solar System package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/frame.h"
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

    FrameTree *parentFrame = system->getSystemTree();
    Frame *defaultOrbitFrame = parentFrame->getDefaultFrame();
    Frame *defaultBodyFrame = parentFrame->getDefaultFrame();

    parentFrame->addObject(body);
    
    Frame *orbitFrame, *bodyFrame;

    // if (!orbitFrameName.empty())
    //     orbitFrame = Frame::create(orbitFrameName, parentObject, parentObject);
    // else
    //     orbitFrame = defaultOrbitFrame;

    // if (!bodyFrameName.empty())
    //     orbitFrame = Frame::create(bodyFrameName, parentObject, parentObject);
    // else
    //     orbitFrame = defaultBodyFrame;

    // fmt::printf("Orbit Frame: %s (Center: %s)\n",
    //     orbitFrame->getsName(), orbitFrame->getCenter()->getsName());
    // fmt::printf("Body Frame: %s (Center: %s)\n",
    //     bodyFrame->getsName(), bodyFrame->getCenter()->getsName());
    
    // body->setOrbitFrame(orbitFrame);
    // body->setObjectFrame(bodyFrame);

    return body;
}