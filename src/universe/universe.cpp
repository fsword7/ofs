// universe.cpp - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "utils/json.h"
#include "engine/player.h"
#include "engine/celestial.h"
#include "engine/vehicle/vehicle.h"
#include "ephem/orbit.h"
#include "ephem/vsop87/vsop87.h"
#include "ephem/sol/luna/elp82.h"
#include "ephem/rotation.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/psystem.h"
#include "universe/astro.h"

#include "main/app.h"
#include "engine/player.h"

void Universe::init()
{
    fs::path homePath = OFS_HOME_DIR;
    stardb.loadXHIPData(homePath / "data/xhip");
    constellations.load(homePath / "data/constellations/western/constellationship.fab");
    // constellations.load("constellations/western_rey/constellationship.fab");
}

void Universe::configure(cjson &config)
{
    if (config["systems"].is_array())
    {
        for (auto &item : config["systems"].items())
        {
            if (!item.value().is_object())
                continue;
            auto entry = item.value();

            str_t sysName;
            fs::path sysFolder;

            sysName = myjson::getString<str_t>(entry, "name");
            sysFolder = myjson::getString<fs::path>(entry, "folder");

            // if (entry["name"].is_string())
            //     sysName = entry["name"].get<str_t>();
            
            // if (entry["folder"].is_string())
            //     sysFolder = entry["folder"].get<fs::path>();
            // sysFolder = OFS_HOME_DIR / sysFolder;

            ofsLogger->info("JSON: Name: {}, Folder: {}\n", sysName, sysFolder.c_str());

            sysFolder = OFS_HOME_DIR / sysFolder;
            if (!pSystem::loadSystem(this, sysName, sysFolder));
        }
    }
}

void Universe::configureVehicles(cjson &config)
{
    if (!config.contains("ships"))
        return;
    if (!config["ships"].is_array())
        return;
    cjson &ships = config["ships"];

    for (int idx = 0; idx < ships.size(); idx++) {
        cjson &ship = ships[idx];
        if (!ship.is_object())
            continue;

        str_t cbTarget = myjson::getString<str_t>(ship, "target");
        Celestial *cbody = nullptr;
        if (!cbTarget.empty())
            cbody = findPath(cbTarget);
        else {
            ofsLogger->error("JSON: Unknown celestial body: {} - aborted\n",
                cbTarget);
            continue;
        }

        pSystem *psys = nullptr;
        if (cbody->hasSystem())
            psys = cbody->getSystem();
        else {
            ofsLogger->error("JSON: {} system does not have solar/planetary system - aborted.\n",
                cbody->getsName());
            continue;
        }

        Vehicle *veh = new Vehicle(ship, cbody);
        psys->addVehicle(veh);
    }
}

void Universe::start()
{
    // Initiializing solar systems with time
    for (auto &psys : systemList)
        psys->update(true);
}

void Universe::update(Player *player, const TimeDate &td)
{
    // Updating periodic close stars
    nearStars.clear();
    findCloseStars(player->getPosition(), 1.0, nearStars);
    // glm::dvec3 pos = player->getPosition();
    // ofsLogger->info("Update: {} nearby stars - Player position: {:f},{:f},{:f}\n",
    //     nearStars.size(), pos.x, pos.y, pos.z);

    // Updating local solar systems
    for (auto sun : nearStars)
    {
        // if (!sun->hasSolarSystem())
        //     continue;
        // System *system = sun->getSolarSystem();

        if (!sun->hasSystem())
            continue;

        // Logger::getLogger()->info("{}: Solar System List\n", sun->getsName());
        pSystem *psys = sun->getSystem();
        if (psys != nullptr)
            psys->update(true);

    }
}

void Universe::finalizeUpdate()
{
    for (auto sun : nearStars)
    {
        if (!sun->hasSystem())
            continue;
        pSystem *psys = sun->getSystem();
        if (psys != nullptr)
            psys->finalizeUpdate();
    }
}

void Universe::addSystem(pSystem *psys)
{
}

pSystem *Universe::createSolarSystem(cstr_t &sysName)
{
    pSystem *psys = getSolarSystem(sysName);
    if (psys != nullptr)
        return psys;
    psys = new pSystem(sysName);
    systems.insert({sysName, psys});
    systemList.push_back(psys);

    return psys;
}

pSystem *Universe::getSolarSystem(cstr_t &sysName) const
{
    if (sysName.empty())
        return nullptr;
    auto iter = systems.find(sysName);
    if (iter != systems.end())
        return iter->second;
    return nullptr;
}

CelestialStar *Universe::findStar(cstr_t &name) const
{
    return stardb.find(name);
}

Celestial *Universe::findObject(const Object *obj, cstr_t &name) const
{
    pSystem *psys;
    const CelestialStar *sun;
    const CelestialBody *body;

    switch (obj->getType())
    {
    case ObjectType::objCelestialStar:
        sun = dynamic_cast<const CelestialStar *>(obj);
        if ((psys = sun->getPlanetarySystem()) == nullptr)
            break;
        return psys->find(name);

    case ObjectType::objCelestialBody:
        body = dynamic_cast<const CelestialBody *>(obj);
        if ((psys = body->getSystem()) == nullptr)
            break;
        return psys->find(name);
    }

    return nullptr;
}

Celestial *Universe::findPath(cstr_t &path) const
{
    std::string::size_type pos = path.find('/', 0);
    if (pos == std::string::npos)
        return findStar(path);

    std::string base(path, 0, pos);
    Celestial *obj = findStar(base);

    while (obj != nullptr && pos != std::string::npos)
    {
        std::string::size_type npos = path.find('/', pos+1);
        std::string::size_type len;

        len = ((npos == std::string::npos) ? path.size() : npos) - pos - 1;
        std::string name = std::string(path, pos+1, len);

        obj = findObject(obj, name);
        pos = npos;
    }

    return obj;
}

int Universe::findCloseStars(const glm::dvec3 &obs, double mdist,
    std::vector<const CelestialStar *> &closeStars) const
{
    return stardb.findCloseStars(obs, mdist, closeStars);
}

void Universe::findVisibleStars(ofsHandler &handler,
    const glm::dvec3 &obs, const glm::dmat3 &rot,
    const double fov, const double aspect,
    const double faintest)
{
    stardb.findVisibleStars(handler, obs, rot, fov, aspect, faintest);
}

bool Universe::pickSystem(secondaries_t &cbodies)
{
    for (auto cbody : cbodies) {


        secondaries_t &secondaries = cbody->getSecondaries();
        if (secondaries.size() > 0)
            pickSystem(secondaries);
    }

    return false;
}

Celestial *Universe::pickPlanet(pSystem *system, const glm::dvec3 &obs, const glm::dvec3 &dir, double when)
{

    return nullptr;
}

Celestial *Universe::pick(const glm::dvec3 &obs, const glm::dvec3 &dir, double when)
{
    Celestial *picked = nullptr;

    for (auto sun : nearStars)
    {
        if (!sun->hasSystem())
            continue;
        picked = pickPlanet(sun->getSystem(), obs, dir, when);
    }

    return picked;
}