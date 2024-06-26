// universe.cpp - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/player.h"
#include "ephem/orbit.h"
#include "ephem/vsop87/vsop87.h"
#include "ephem/lunar/elp82.h"
#include "ephem/rotation.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "universe/body.h"
// #include "universe/system.h"
#include "universe/psystem.h"
#include "universe/astro.h"

#include "main/app.h"
#include "engine/player.h"

void Universe::init()
{
    stardb.loadXHIPData("data/xhip");
    constellations.load("data/constellations/western/constellationship.fab");
    // constellations.load("constellations/western_rey/constellationship.fab");

    if (!pSystem::loadSystems(this, "Sol"))
        exit(1);
}

void Universe::start(const TimeDate &td)
{
    Camera *cam = ofsAppCore->getCamera();
    Player *player = ofsAppCore->getPlayer();

    CelestialStar *sun = stardb.find("Sol");
    pSystem *psys = sun->getpSystem();
    CelestialBody *earth = psys->find("Earth");
    CelestialBody *lunar = psys->find("Lunar");

    assert(sun != nullptr);
    assert(psys != nullptr);
    assert(earth != nullptr);
    assert(lunar != nullptr);

    // cam->setPosition({ 0, 0, -sun->getRadius() * 4.0 });
    // player->attach(sun, camTargetRelative);
    // player->look(sun);

    // cam->setPosition({ 0, 0, mercury->getRadius() * 4.0 });
    // player->attach(mercury, camTargetRelative);
    // player->look(mercury);

    // cam->setPosition({ 0, 0, mars->getRadius() * 4.0 });
    // player->attach(mars, camTargetRelative);
    // player->look(mars);

    // cam->setPosition({ 0, 0, jupiter->getRadius() * 4.0 });
    // player->attach(jupiter, camTargetRelative);
    // player->look(jupiter);

    // cam->setPosition(earth->convertEquatorialToLocal(
    //     glm::radians(28.632307), glm::radians(-80.705774), earth->getRadius()+50));
    // // cam->setPosition({ 0, 0, earth->getRadius() * 4.0 });
    // player->attach(earth, camTargetRelative);
    // player->look(earth);

    // On Runway 15 (Cape Kennedy) - 28.632307, -80.705774
    player->setGroundObserver(earth, { 28.632307, -80.705774, 1}, 150);

    // Observe Rocky Mountains in Denver metro area
    // player->setGroundObserver(earth, { 39.7309918, -104.7046216, 5}, 270);

    // Observe Grand Canyon in Arizona
    // player->setGroundObserver(earth, { 36.018679, -112.121223, 5}, 0);

    // Observe Hawaiian islands
    // player->setGroundObserver(earth, { 21.059613, -157.957629, 3}, 0);

    // cam->setPosition({ 0, 0, lunar->getRadius() * 4.0});
    // player->attach(lunar, camTargetRelative);
    // player->look(lunar);

    // Ground observer on Lunar
    // player->setGroundObserver(lunar, { 0, 0, 3 }, 0);

    player->update(td);
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

        if (!sun->haspSystem())
            continue;

        // Logger::getLogger()->info("{}: Solar System List\n", sun->getsName());
        pSystem *psys = sun->getpSystem();
        if (psys != nullptr)
            psys->update(td);

    }
}

void Universe::finalizeUpdate()
{
    for (auto sun : nearStars)
    {
        if (!sun->haspSystem())
            continue;
        pSystem *psys = sun->getpSystem();
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

Object *Universe::findObject(const Object *obj, cstr_t &name) const
{
    System *system;
    const CelestialStar *sun;
    const CelestialBody *body;
    // const PlanetarySystem *objects;

    switch (obj->getType())
    {
    case ObjectType::objCelestialStar:
        sun = dynamic_cast<const CelestialStar *>(obj);
        // if ((system = sun->getSolarSystem()) == nullptr)
        //     break;
        // objects = system->getPlanetarySystem();
        // return objects->find(name);
        break;

    case ObjectType::objCelestialBody:
        body = dynamic_cast<const CelestialBody *>(obj);
        // objects = body->getOwnSystem();
        // if (objects != nullptr)
        //     return objects->find(name);
        break;
    }

    return nullptr;
}

Object *Universe::findPath(cstr_t &path) const
{
    std::string::size_type pos = path.find('/', 0);
    if (pos == std::string::npos)
        return findStar(path);

    std::string base(path, 0, pos);
    Object *obj = findStar(base);

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

Object *Universe::pickPlanet(pSystem *system, const glm::dvec3 &obs, const glm::dvec3 &dir, double when)
{

    return nullptr;
}

Object *Universe::pick(const glm::dvec3 &obs, const glm::dvec3 &dir, double when)
{
    Object *picked = nullptr;

    for (auto sun : nearStars)
    {
        if (!sun->haspSystem())
            continue;
        picked = pickPlanet(sun->getpSystem(), obs, dir, when);
    }

    return picked;
}