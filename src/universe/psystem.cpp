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

bool pSystem::loadPlanet(const cstr_t &cbName, pSystem *psys, fs::path cbFolder)
{
    ofsLogger->info("Loading planet {}...\n", cbName);
    str_t fileName = cbName + ".json";

    std::ifstream jFile(cbFolder / fileName);
    if (!jFile.is_open())
    {
        ofsLogger->info("Can't open {} json file: {}\n", cbName, strerror(errno));
        return false;
    }
    json data = json::parse(jFile);
    jFile.close();

    // for (auto item : data.items())
    // {
    //     ofsLogger->info("Item {}: \n", item.key());
    //     // if (!item.key().find("Planet"))
    //     // {
    //     //     if (!item.value().is_string())
    //     //         continue;
    //     //     auto cbName = item.value().get<std::string>();
    //     //     fs::path cbFolder = sysFolder / cbName;
    //     //     loadPlanet(cbName, psys, cbFolder);
    //     // }
    // }

    return false;
}

bool pSystem::loadSystems(Universe *universe, const cstr_t &sysName)
{
    ofsLogger->info("Loading {} system...\n", sysName);

    fs::path sysFolder = "data/systems/" + sysName;
    str_t fileName = sysName + ".json";

    std::ifstream jFile(sysFolder / fileName);
    if (!jFile.is_open())
    {
        ofsLogger->info("Can't open {} json file: {}\n", sysName, strerror(errno));
        return false;
    }
    json data = json::parse(jFile);
    // std::cout << std::setw(4) << data << "\n\n";
    jFile.close();

    auto sys = data["System"];
    std::string name;
    if (sys.is_string())
        name = sys.get<std::string>();

    StarDatabase &stardb = universe->getStarDatabase();
    CelestialStar *sun = stardb.find(name);
    if (sun == nullptr)
    {
        ofsLogger->info("System: {}: star not found\n", name);
        return false;
    }
    pSystem *psys = new pSystem(sun);

    for (auto item : data.items())
    {
        // ofsLogger->info("Item {}: \n", item.key());
        if (!item.key().find("Planet"))
        {
            if (!item.value().is_string())
                continue;
            auto cbName = item.value().get<std::string>();
            fs::path cbFolder = sysFolder / cbName;
            loadPlanet(cbName, psys, cbFolder);
        }
    }
    return true;
}