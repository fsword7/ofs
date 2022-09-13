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
#include "ephem/vsop87.h"
#include "ephem/rotation.h"
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

    Logger::getLogger()->info("Body {} -> {}\n", name, parentObject->getsName());

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

    Logger::getLogger()->info("Orbit Frame: {} (Center: {})\n",
        orbitFrame->getsName(), orbitFrame->getCenter()->getsName());
    Logger::getLogger()->info("Body Frame: {} (Center: {})\n",
        bodyFrame->getsName(), bodyFrame->getCenter()->getsName());
    
    body->setOrbitFrame(orbitFrame);
    body->setBodyFrame(bodyFrame);

    return body;
}

bool System::logError(const Parser &parser, cstr_t &message)
{
    Logger::getLogger()->error("Error in .sso file (line {}): {}\n",
        parser.getLineNumber(), message);
    return false;
}

typedef std::map<std::string, celType, CompareIgnoreCasePredicate> classificationTable;
static classificationTable classifications;

void System::initClassifications()
{
    // Build classification table
    classifications["planet"] = celType::cbPlanet;
    classifications["dwarfplanet"] = celType::cbDwarfPlanet;
    classifications["moon"] = celType::cbMoon;
    classifications["submoon"] = celType::cbSubmoon;
    classifications["asteroid"] = celType::cbAsteroid;
    classifications["comet"] = celType::cbComet;
}

celType System::getClassification(cstr_t &className)
{
    if (classifications.empty())
        initClassifications();
    
    classificationTable::iterator iter = classifications.find(className);
    if (iter == classifications.end())
        return celType::cbUnknown;
    return iter->second;
}

void System::setSurface(celSurface &surface, Group *objData)
{

    // Color and lighting parameters
    objData->getColor("Color", surface.color);
    objData->getColor("SpecularColor", surface.spColor);
    objData->getNumber("SpecularPower", surface.spPower);

}

Object *System::getFrameCenter(Universe &universe, Group *gFrame, Object *defaultObject)
{
    std::string centerName;
    if (!gFrame->getString("Center", centerName))
        return defaultObject;
    
    Object *centerObject = universe.findPath(centerName);
    if (centerObject == nullptr)
    {
        Logger::getLogger()->error("Bad center definition");
        return nullptr;
    }

    return centerObject;
}

Frame *System::createJ2000EclipticFrame(Universe &universe, Group *gFrame, Object *defaultCenter)
{
    Object *centerObject = getFrameCenter(universe, gFrame, defaultCenter);
    if (centerObject == nullptr)
    {
        Logger::getLogger()->error("Bad J2000 Ecliptic frame definition");
        return nullptr;
    }

    return new J2000EclipticFrame(centerObject);
}

Frame *System::createJ2000EquatorFrame(Universe &universe, Group *gFrame, Object *defaultCenter)
{
    Object *centerObject = getFrameCenter(universe, gFrame, defaultCenter);
    if (centerObject == nullptr)
    {
        Logger::getLogger()->error("Bad J2000 Equator frame definition");
        return nullptr;
    }

    return new J2000EquatorFrame(centerObject);
}

Frame *System::createReferenceFrame(Universe &universe, Group *gFrame, Object *center, Object *body)
{
    Value *value = nullptr;

    if (value = gFrame->getValue("EclipticJ2000"))
    {
        if (value->getType() != Value::vtGroup)
        {
            Logger::getLogger()->error("Bad J2000 Ecliptic reference frame definition");
            return nullptr;
        }

        return createJ2000EclipticFrame(universe, value->getGroup(), center);
    }

    if (value = gFrame->getValue("EquatorJ2000"))
    {
        if (value->getType() != Value::vtGroup)
        {
            Logger::getLogger()->error("Bad J2000 Equator reference frame definition");
            return nullptr;
        }

        return createJ2000EquatorFrame(universe, value->getGroup(), center);
    }

    return nullptr;
}

Frame *System::createReferenceFrame(Universe &universe, Value *vFrame, Object *center, Object *body)
{
    if (vFrame->getType() != Value::vtGroup)
    {
        Logger::getLogger()->error("Bad reference frame definition");
        return nullptr;
    }

    return createReferenceFrame(universe, vFrame->getGroup(), center, body);
}

Orbit *System::createOrbit(Object *centerObject, Group *objData, const fs::path &path)
{
    Orbit *orbit = nullptr;

    std::string orbitName;
    if (objData->getString("Orbit", orbitName))
    {
        orbit = VSOP87Orbit::create(orbitName);
        if (orbit != nullptr)
            return orbit;
        Logger::getLogger()->error("Can't find VSOP07 orbit name '{}'\n", orbitName);
    }

    return nullptr;
}

Rotation *System::createRotation(Object *centerObject, Group *objData, const fs::path &path)
{
    Rotation *rotation = nullptr;

    std::string rotationName;
    if (objData->getString("Rotation", rotationName))
    {
        rotation = Rotation::create(rotationName);
        if (rotation != nullptr)
            return rotation;
        Logger::getLogger()->error("Can't find rotation name '{}'\n", rotationName);
    }

    return nullptr;
}

celBody *System::createBody2(cstr_t &name, celType type, PlanetarySystem *pSystem, Universe &universe, Group *objData)
{
    // Determine body classification first
    std::string className;
    celType bodyType = celType::cbUnknown;
    if (objData->getString("Class", className));
        bodyType = getClassification(className);

    celBody *body = new celBody(pSystem, name, bodyType);


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

    Frame *orbitFrame = nullptr;
    Frame *bodyFrame = nullptr;
    Orbit *orbit = nullptr;
    Rotation *rotation = nullptr;
    Object *parent = nullptr;

    // Default timeline without limits
    double beginning = -std::numeric_limits<double>::infinity();
    double ending = std::numeric_limits<double>::infinity();

    // Get the object's orbit reference frame
    Value *vOrbitFrame = objData->getValue("OrbitFrame");
    if (vOrbitFrame != nullptr)
    {
        auto frame = createReferenceFrame(universe, vOrbitFrame, parent, body);
        if (frame != nullptr)
            orbitFrame = frame;
    }

    // the object's body frame
    Value *vBodyFrame = objData->getValue("BodyFrame");
    if (vBodyFrame != nullptr)
    {
        auto frame = createReferenceFrame(universe, vBodyFrame, parent, body);
        if (frame != nullptr)
            bodyFrame = frame;
    }

    orbit = createOrbit(orbitFrame->getCenter(), objData, "");
    rotation = createRotation(bodyFrame->getCenter(), objData, "");

    body->setOrbitFrame(orbitFrame);
    body->setBodyFrame(bodyFrame);
    body->setOrbit(orbit);
    body->setRotation(rotation);

    return body;
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

        Logger::getLogger()->info("Body: {} Parent: {} Object: {}\n", bodyName, parentName,
            parent != nullptr ? parent->getsName() : "<Body not found>");

        if (parent->getType() == ObjectType::objCelestialStar)
        {
            celStar *star = dynamic_cast<celStar *>(parent);
            System *system = universe.createSolarSystem(star);
            pSystem = system->getPlanetarySystem();
        }
        else if (parent->getType() == ObjectType::objCelestialBody)
        {
            celBody *body = dynamic_cast<celBody *>(parent);
            pSystem = body->createPlanetarySystem();
        }
        else
            return logError(parser, fmt::format("Parent celestial body '{}' of '{}' not found",
                parentName, primaryName));

        celBody *existingBody = pSystem->find(primaryName);
        if (existingBody != nullptr)
        {
            logError(parser, fmt::format("Body '{}' already created", primaryName));
            delete objData;
            continue;
        }

        celBody *body = createBody2(primaryName, celType::cbUnknown, pSystem, universe, objData);

        // All done, delete all object definitions
        delete group;
    }

    return false;
}