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

pSystem::pSystem(CelestialStar *star)
: primaryStar(star)
{
    star->setpSystem(this);
    stars.push_back(star);
    bodies.push_back(star);
}

void pSystem::addStar(CelestialStar *star)
{
    if (!star->haspSystem())
        star->setpSystem(this);
    stars.push_back(star);
}

void pSystem::addBody(CelestialBody *cbody)
{
    bodies.push_back(cbody);
}

void pSystem::addGravity(CelestialBody *grav)
{
    gravities.push_back(grav);
}

void pSystem::addPlanet(CelestialBody *planet, CelestialBody *cbody)
{
    planets.push_back(planet);
    addBody(planet);
    addGravity(planet);
    planet->setStar(primaryStar);
    planet->setPlanetarySystem(this);
    planet->attach(cbody);
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

// bool pSystem::loadPlanet(const cstr_t &cbName, pSystem *psys, fs::path cbFolder)
// {
//     ofsLogger->info("Loading planet {}...\n", cbName);
//     str_t fileName = cbName + ".json";

//     std::ifstream jFile(cbFolder / fileName);
//     if (!jFile.is_open())
//     {
//         ofsLogger->info("Can't open {} json file: {}\n", cbName, strerror(errno));
//         return false;
//     }
//     json data = json::parse(jFile, nullptr, true, true);
//     jFile.close();

//     // for (auto item : data.items())
//     // {
//     //     ofsLogger->info("Item {}: \n", item.key());
//     //     // if (!item.key().find("Planet"))
//     //     // {
//     //     //     if (!item.value().is_string())
//     //     //         continue;
//     //     //     auto cbName = item.value().get<std::string>();
//     //     //     fs::path cbFolder = sysFolder / cbName;
//     //     //     loadPlanet(cbName, psys, cbFolder);
//     //     // }
//     // }

//     return false;
// }

// bool pSystem::loadSystems(Universe *universe, cstr_t &sysName)
// {
//     ofsLogger->info("Loading {} system...\n", sysName);

//     fs::path sysFolder = "data/systems/" + sysName;
//     str_t fileName = sysName + ".json";

//     std::ifstream jFile(sysFolder / fileName);
//     if (!jFile.is_open())
//     {
//         ofsLogger->info("Can't open {} json file: {}\n", sysName, strerror(errno));
//         return false;
//     }
//     json data = json::parse(jFile, nullptr, true, true);
//     // std::cout << std::setw(4) << data << "\n\n";
//     jFile.close();

//     auto sys = data["System"];
//     std::string name;
//     if (sys.is_string())
//         name = sys.get<std::string>();

//     StarDatabase &stardb = universe->getStarDatabase();
//     CelestialStar *sun = stardb.find(name);
//     if (sun == nullptr)
//     {
//         ofsLogger->info("System: {}: star not found\n", name);
//         return false;
//     }
//     pSystem *psys = new pSystem(sun);

//     for (auto item : data.items())
//     {
//         // ofsLogger->info("Item {}: \n", item.key());
//         if (!item.key().find("Planet"))
//         {
//             if (!item.value().is_string())
//                 continue;
//             auto cbName = item.value().get<std::string>();
//             fs::path cbFolder = sysFolder / cbName;
//             loadPlanet(cbName, psys, cbFolder);
//         }
//     }
//     return true;
// }

struct {
    const char *name;
    celType type;
} celTypes[2] = {
    { "planet", cbPlanet },
    { "moon", cbMoon }
};

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

    if (config["System"].IsScalar())
    {
        std::string sysName = config["System"].as<std::string>();
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
    delete cbody;

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

    StarDatabase &stardb = universe->getStarDatabase();
    CelestialStar *sun = stardb.find(name);
    if (sun == nullptr)
    {
        ofsLogger->info("System: {}: star not found\n", name);
        return false;
    }
    pSystem *psys = new pSystem(sun);
    ofsLogger->info("Creating {} planetary system...\n", name);

    for (auto planet : config["planets"])
    {
        // ofsLogger->info("Planet: {}\n", planet.as<std::string>());
        str_t cbName = planet.as<std::string>();
        str_t cbFolder = sysFolder + cbName + "/";
        loadPlanet(cbName, psys, cbFolder);
    }

    return true;
}