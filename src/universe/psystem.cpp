// psystem.cpp - Planetary System package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#include "main/core.h"
#include "universe/universe.h"
#include "universe/celbody.h"
#include "universe/body.h"
#include "universe/star.h"
#include "universe/psystem.h"

#include "yaml-cpp/yaml.h"

pSystem::pSystem(str_t &name)
: sysName(name)
{
}

pSystem::pSystem(CelestialStar *star)
: sysName(star->getsName()), primaryStar(star)
{
    star->setpSystem(this);
    stars.push_back(star);
    bodies.push_back(star);
}

void pSystem::addStar(CelestialStar *star)
{
    if (primaryStar == nullptr)
        primaryStar = star;
    if (!star->haspSystem())
        star->setpSystem(this);
    stars.push_back(star);
    bodies.push_back(star);
}

void pSystem::addBody(CelestialBody *cbody)
{
    bodies.push_back(cbody);
}

void pSystem::addGravity(CelestialBody *grav)
{
    gravities.push_back(grav);
}

void pSystem::addPlanet(CelestialBody *planet, CelestialBody *parent)
{
    planets.push_back(planet);
    addBody(planet);
    addGravity(planet);
    planet->setStar(primaryStar);
    planet->setPlanetarySystem(this);
    planet->attach(parent);
}

CelestialBody *pSystem::find(cstr_t &name) const
{
    for (auto body : bodies)
        if (body->getsName() == name)
            return body;
    return nullptr;
}

void pSystem::update(const TimeDate &td)
{

    for (auto star : stars)
        star->updateEphemeris(td);
    for (auto star : stars)
        star->updatePostEphemeris(td);   
}

void pSystem::finalizeUpdate()
{
    for (auto body : bodies)
        body->endUpdate();
}

struct {
    const char *name;
    celType type;
} celTypes[2] = {
    { "planet", cbPlanet },
    { "moon", cbMoon }
};

bool pSystem::loadStar(const cstr_t &cbName, Universe *universe, pSystem *psys, cstr_t &cbFolder)
{
    ofsLogger->info("Loading {}...\n", cbName);
    str_t fileName = cbName + ".yaml";

    YAML::Node config;
    config = YAML::LoadFile(cbFolder + fileName);

    if (config["Activate"].IsScalar())
    {
        if (config["Activate"].as<bool>() == false)
        {
            ofsLogger->info("OFS Info: Disabled celestial body: {}\n",
                cbName);
            return false;
        }
    }

    StarDatabase &stardb = universe->getStarDatabase();
    CelestialStar *star = stardb.find(cbName);
    if (star == nullptr)
    {
        ofsLogger->info("OFS: star {} not found\n", cbName);
        return false;
    }

    // str_t sysName;
    // CelestialBody *parent = nullptr;
    // if (config["System"].IsScalar())
    // {
    //     sysName = config["System"].as<std::string>();
    //     parent = psys->find(sysName);
    //     if (parent == nullptr)
    //     {
    //         ofsLogger->error("OFS Error: Unknown system: {}\n", sysName);
    //         return false;
    //     }
    // }

    // celType type = cbUnknown;
    // if (config["Type"].IsScalar())
    // {
    //     std::string typeName = config["Type"].as<std::string>();
    //     for (auto ctype : celTypes)
    //         if (ctype.name == typeName)
    //             type = ctype.type;
    //     if (type == cbUnknown)
    //     {
    //         ofsLogger->info("OFS Error: Unknown celestial body type: {}\n",
    //             typeName);
    //         return false;
    //     }
    // }

    star->configure(config);
    psys->addStar(star);

    return true;
}

bool pSystem::loadPlanet(const cstr_t &cbName, pSystem *psys, cstr_t &cbFolder)
{
    ofsLogger->info("Loading {}...\n", cbName);
    str_t fileName = cbName + ".yaml";

    YAML::Node config;
    config = YAML::LoadFile(cbFolder + fileName);

    if (config["Activate"].IsScalar())
    {
        if (config["Activate"].as<bool>() == false)
        {
            ofsLogger->info("OFS Info: Disabled celestial body: {}\n",
                cbName);
            return false;
        }
    }

    CelestialBody *parent = nullptr;
    if (config["System"].IsScalar())
    {
        std::string sysName = config["System"].as<std::string>();
        parent = psys->find(sysName);
        if (parent == nullptr)
        {
            ofsLogger->error("OFS Error: Unknown system: {}\n", sysName);
            return false;
        }
    }

    celType type = cbUnknown;
    if (config["Type"].IsScalar())
    {
        std::string typeName = config["Type"].as<std::string>();
        for (auto ctype : celTypes)
            if (ctype.name == typeName)
                type = ctype.type;
        if (type == cbUnknown)
        {
            ofsLogger->info("OFS Error: Unknown celestial body type: {}\n",
                typeName);
            return false;
        }
    }

    CelestialPlanet *cbody = new CelestialPlanet(config, type);

    psys->addPlanet(cbody, parent);

    return true;
}

bool pSystem::loadSystems(Universe *universe, cstr_t &sysName)
{
    ofsLogger->info("Loading {} system...\n", sysName);

    str_t sysFolder = "data/systems/" + sysName + "/";
    str_t fileName = sysName + ".yaml";

    YAML::Node config;

    try {
        config = YAML::LoadFile(sysFolder + fileName);
    }
    catch (YAML::Exception &e)
    {
        ofsLogger->info("Error: {}\n", e.what());
        return false;
    }

    // for (auto it = config.begin(); it != config.end(); ++it)
    // {
    //     std::string key = it->first.as<std::string>();
    //     YAML::Node value = it->second;
    //     ofsLogger->info("Key: {}:", key);
    //     switch (value.Type())
    //     {
    //     case YAML::NodeType::Null:
    //         ofsLogger->info(" (Null)\n");
    //         break;
    //     case YAML::NodeType::Scalar:
    //         ofsLogger->info(" (Scalar) {}\n",
    //             value.as<std::string>());
    //         break;
    //     case YAML::NodeType::Sequence:
    //         ofsLogger->info(" (Sequence)\n");
    //         break;
    //     case YAML::NodeType::Map:
    //         ofsLogger->info(" (Map)\n");
    //         break;
    //     case YAML::NodeType::Undefined:
    //         ofsLogger->info(" (Undefined)\n");
    //         break;
    //     }
    // }

    YAML::Node sys = config["system"];
    std::string name;
    if (sys.IsScalar())
        name = sys.as<std::string>();

    // StarDatabase &stardb = universe->getStarDatabase();
    // CelestialStar *sun = stardb.find(name);
    // if (sun == nullptr)
    // {
    //     ofsLogger->info("System: {}: star not found\n", name);
    //     return false;
    // }
    pSystem *psys = new pSystem(name);
    // pSystem *psys = new pSystem(sun);
    ofsLogger->info("Creating {} planetary system...\n", name);

    for (auto body : config["stars"])
    {
        // ofsLogger->info("Star: {}\n", planet.as<std::string>());
        str_t cbName = body.as<std::string>();
        str_t cbFolder = sysFolder + cbName + "/";
        loadStar(cbName, universe, psys, cbFolder);
    }

    for (auto body : config["planets"])
    {
        // ofsLogger->info("Planet: {}\n", planet.as<std::string>());
        str_t cbName = body.as<std::string>();
        str_t cbFolder = sysFolder + cbName + "/";
        loadPlanet(cbName, psys, cbFolder);
    }

    return true;
}