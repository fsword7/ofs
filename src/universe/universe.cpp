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

void Universe::configure(const json &config)
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

void Universe::start(const TimeDate &td)
{
    // Camera *cam = ofsAppCore->getCamera();
    // Player *player = ofsAppCore->getPlayer();

    // CelestialStar *sun = dynamic_cast<CelestialStar *>(stardb.find("Sol"));
    // pSystem *psys = sun->getSystem();
    // Celestial *earth = dynamic_cast<Celestial *>(psys->find("Earth"));
    // Celestial *luna = dynamic_cast<Celestial *>(psys->find("Luna"));
    // Celestial *mars = dynamic_cast<Celestial *>(psys->find("Mars"));
    // Celestial *mercury = dynamic_cast<Celestial *>(psys->find("Mercury"));
    // Celestial *jupiter = dynamic_cast<Celestial *>(psys->find("Jupiter"));
  
    // vehicle = new Vehicle();
    // psys->addVehicle(vehicle);

    // assert(sun != nullptr);
    // assert(psys != nullptr);
    // assert(earth != nullptr);
    // assert(luna != nullptr);
    // assert(mars != nullptr);
    // assert(mercury != nullptr);
    // assert(jupiter != nullptr);

    // cam->setPosition({ 0, 0, -sun->getRadius() * 4.0 });
    // player->attach(sun, camTargetRelative);
    // player->look(sun);

    // cam->setPosition({ 0, 0, mercury->getRadius() * 4.0 });
    // player->attach(mercury, camTargetSync, sun);
    // player->look(mercury);

    // cam->setPosition({ 0, 0, mars->getRadius() * 4.0 });
    // player->attach(mars, camTargetSync, sun);
    // player->look(mars);

    // cam->setPosition({ 0, 0, jupiter->getRadius() * 4.0 });
    // player->attach(jupiter, camTargetSync, sun);
    // player->look(jupiter);

    // cam->setPosition(earth->convertEquatorialToLocal(
    //     glm::radians(28.632307), glm::radians(-80.705774), earth->getRadius()+50));
    // cam->setPosition({ 0, 0, earth->getRadius() * 4.0 });
    // player->attach(earth, camTargetRelative);
    // player->look(earth);

    // cam->setPosition({ 0, 0, earth->getRadius() * 4.0 });
    // player->attach(earth, camTargetUnlocked);
    // player->look(earth);

    // cam->setPosition({ 0, 0, earth->getRadius() * 4.0 });
    // player->attach(earth, camTargetSync, sun);
    // player->look(earth);

    // On Runway 15 (Cape Kennedy) - 28.632307, -80.705774
    // player->setGroundObserver(earth, { 28.632307, -80.705774, 1}, 150);
    // player->setGroundObserver(earth, { 28.632307, -80.705774, .003}, 150);

    // vehicle->initLanded(earth, { 28.632307, -80.705774, 0}, 150);
    // player->attach(vehicle);

    // Observe Rocky Mountains in Denver metro area
    // player->setGroundObserver(earth, { 39.7309918, -104.7046216, 2}, 270);

    // Observe Grand Canyon in Arizona
    // player->setGroundObserver(earth, { 36.018679, -112.121223, 5}, 0);

    // Observe Hawaiian islands
    // player->setGroundObserver(earth, { 21.059613, -157.957629, 3}, 0);

    // cam->setPosition({ 0, 0, lunar->getRadius() * 4.0});
    // player->attach(lunar, camTargetSync);
    // player->look(lunar);

    // cam->setPosition({ 0, 0, lunar->getRadius() * 4.0});
    // player->attach(lunar, camTargetSync, sun);
    // player->look(lunar);

    // cam->setPosition({ 0, 0, lunar->getRadius() * 4.0});
    // player->attach(lunar, camTargetSync, earth);
    // player->look(lunar);

    // Ground observer on Lunar
    // player->setGroundObserver(lunar, { 0, 0, 3 }, 0);

    // player->setSystem(psys);
    // player->update(td);
}

void Universe::update(Player *player, const TimeDate &td)
{
    // Updating periodic close stars
    nearStars.clear();
    findCloseStars(player->getPosition(), 1.0, nearStars);

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

// System *Universe::createSolarSystem(CelestialStar *star)
// {
//     System *system = getSolarSystem(star);
//     if (system != nullptr)
//         return system;

//     auto idStar = star->getHIPnumber();
//     system = new System(star);
//     systems.insert({idStar, system});

//     return system;
// }

// System *Universe::getSolarSystem(CelestialStar *star) const
// {
//     if (star == nullptr)
//         return nullptr;
//     auto idStar = star->getHIPnumber();
//     auto iter = systems.find(idStar);
//     if (iter != systems.end())
//         return iter->second;
//     return nullptr;
// }

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