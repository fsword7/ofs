// system.cpp - Solar System package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#include "main/core.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/frame.h"
#include "universe/system.h"
#include "universe/universe.h"
#include "scripts/parser.h"

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

    if (!orbitFrameName.empty())
        orbitFrame = Frame::create(orbitFrameName, parentObject, parentObject);
    else
        orbitFrame = defaultOrbitFrame;

    if (!bodyFrameName.empty())
        bodyFrame = Frame::create(bodyFrameName, parentObject, parentObject);
    else
        bodyFrame = defaultBodyFrame;

    fmt::printf("Orbit Frame: %s (Center: %s)\n",
        orbitFrame->getsName(), orbitFrame->getCenter()->getsName());
    fmt::printf("Body Frame: %s (Center: %s)\n",
        bodyFrame->getsName(), bodyFrame->getCenter()->getsName());
    
    body->setOrbitFrame(orbitFrame);
    body->setBodyFrame(bodyFrame);

    return body;
}

bool System::logError(const Parser &parser, cstr_t &message)
{
    fmt::printf("Error in .sso file (line %d): %s\n",
        parser.getLineNumber(), message);
    return false;
}

void System::setSurface(celSurface &surface, Group *objData)
{

    // Color and lighting parameters
    objData->getColor("Color", surface.color);
    objData->getColor("SpecularColor", surface.spColor);
    objData->getNumber("SpecularPower", surface.spPower);

}

celBody *System::createBody2(cstr_t &name, celType type, PlanetarySystem *pSystem, Group *objData)
{
    celBody *body = new celBody(pSystem, name, cbUnknown);


    double val;
    if (objData->getNumber("Nass", val))
        body->setMass(val);
    if (objData->getNumber("Radius", val))
        body->setRadius(val);
    if (objData->getNumber("Albedo", val))
        body->setAlbedo(val);

    celSurface surface;
    setSurface(surface, objData);
    // body->setSurface(surface);

    return nullptr;
}

bool System::loadSolarSystemObjects(std::istream &in, Universe &universe, const fs::path &path)
{
    Parser parser(in);

    while (parser.getNextToken() != tkEnd)
    {
        // Usage:
        //      body <name list> <parent>
        //      {
        //          :
        //          :
        //      }
        //
        //  Where:
        //      Name list = "name1[:name2:...]"

        std::string itemType("Body");
        if (parser.getTokenType() == tkName)
            itemType = parser.getText();
        else
            return logError(parser, "Body expected");
        if (itemType != "Body")
            return logError(parser, "Body expected");

        // Obtain body name
        std::string bodyName;
        if (parser.getNextToken() != tkString)
            return logError(parser, "Object name expected");
        bodyName = parser.getText();

        // Obtain parent name
        std::string parentName;
        if (parser.getNextToken() != tkString)
            return logError(parser, "Parent name expected");
        parentName = parser.getText();

        // Obtain group list
        Value *group = parser.getValue();
        if (group == nullptr)
            return logError(parser, "Bad object definition");
        if (group->getType() != Value::vtGroup)
        {
            logError(parser, "Begin '{' expected");
            delete group;
            return false;
        }
        Group *objData = group->getGroup();

        // Break body name down with colon delimiter for names list
        std::vector<std::string> nameList;
        std::string::size_type startPos = 0;
        while (startPos != std::string::npos)
        {
            std::string::size_type next = bodyName.find(':', startPos);
            std::string::size_type length = std::string::npos;
            if (next != std::string::npos)
            {
                length = next - startPos;
                ++next;
            }
            nameList.push_back(bodyName.substr(startPos, length));
            startPos = next;
        }

        Object *parent = universe.findPath(parentName);
        PlanetarySystem *pSystem = nullptr;
        std::string primaryName = nameList.front();

        fmt::printf("Body: %s Parent: %s Object: %s\n", bodyName, parentName,
            parent != nullptr ? parent->getsName() : "<Body not found>");

        if (parent->getType() == Object::objCelestialStar)
        {
            celStar *star = dynamic_cast<celStar *>(parent);
            System *system = universe.createSolarSystem(star);
            pSystem = system->getPlanetarySystem();
        }
        else if (parent->getType() == Object::objCelestialBody)
        {
            celBody *body = dynamic_cast<celBody *>(parent);
            pSystem = body->createPlanetarySystem();
        }
        else
            return logError(parser, fmt::sprintf("Parent celestial body '%s' of '%s' not found",
                parentName, primaryName));

        celBody *existingBody = pSystem->find(primaryName);
        if (existingBody != nullptr)
        {
            logError(parser, fmt::sprintf("Body '%s' already created", primaryName));
            delete objData;
            continue;
        }

        celBody *body = createBody2(primaryName, celType::cbUnknown, pSystem, objData);

        // All done, delete all object definitions
        delete group;
    }

    return false;
}