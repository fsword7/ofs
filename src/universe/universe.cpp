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
#include "scripts/parser.h"

#include "main/app.h"
#include "engine/player.h"

void Universe::init()
{
    stardb.loadXHIPData("data/xhip");
    constellations.load("data/constellations/western/constellationship.fab");
    // constellations.load("constellations/western_rey/constellationship.fab");

    // CelestialStar *sun = stardb.find("Sol");
    sun = stardb.find("Sol");

    pSystem *psys = new pSystem(sun);
    // System *solSystem = createSolarSystem(sun);
    // PlanetarySystem *system = solSystem->getPlanetarySystem();

    // CelestialBody *mercury, *venus, *earth;
    // CelestialBody *mars, *jupiter, *saturn;
    // CelestialBody *uranus, *neptune, *lunar;

    // mercury = System::createBody("Mercury", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // venus = System::createBody("Venus", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // earth = System::createBody("Earth", system, celType::cbPlanet,
    //     "EclipticJ2000", "EclipticJ2000");
    // mars = System::createBody("Mars", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // jupiter = System::createBody("Jupiter", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // saturn = System::createBody("Saturn", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // uranus = System::createBody("Uranus", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // neptune = System::createBody("Neptune", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");

    // system = earth->createPlanetarySystem();
    // lunar = System::createBody("Moon", system, celType::cbMoon,
    //     "EclipticJ2000", "EquatorJ2000");

    mercury = new CelestialPlanet("Mercury", cbPlanet);
    // venus = new CelestialPlanet("Venus", cbPlanet);
    earth = new CelestialPlanet("Earth", cbPlanet);
    lunar = new CelestialPlanet("Lunar", cbMoon);
    mars = new CelestialPlanet("Mars", cbPlanet);
    jupiter = new CelestialPlanet("Jupiter", cbPlanet);
    // saturn = new CelestialPlanet("Saturn", cbPlanet);
    // uranus = new CelestialPlanet("Uranus", cbPlanet);
    // neptune = new CelestialPlanet("Neptune", cbPlanet);

    sun->setEphemeris(OrbitVSOP87::create(*sun, "vsop87e-sol"));

    mercury->setEphemeris(OrbitVSOP87::create(*mercury, "vsop87b-mercury"));
    mercury->setMass(6.418542e+23);
    mercury->setRadius(3389.5);
    mercury->setAlbedo(0.250);
    mercury->setColor(color_t(0.52, 0.36, 0.16));

    // venus->setEphemeris(OrbitVSOP87::create(*venus, "vsop87b-venus"));
    // venus->setMass(6.418542e+23);
    // venus->setRadius(3389.5);
    // venus->setAlbedo(0.250);
    // venus->setColor(color_t(0.52, 0.36, 0.16));

    earth->setEphemeris(OrbitVSOP87::create(*earth, "vsop87b-earth"));
    // earth->setRotation(Rotation::create("p03lp-earth"));
    // earth->setRotation(Rotation::create("iau-earth"));
    earth->setMass(5.9722e+24);
    earth->setRadius(6378.140);
    earth->setAlbedo(0.449576);
    earth->setColor(color_t(0.856, 0.910, 1.0));

    mars->setEphemeris(OrbitVSOP87::create(*mars, "vsop87b-mars"));
    mars->setMass(6.418542e+23);
    mars->setRadius(3389.5);
    mars->setAlbedo(0.250);
    mars->setColor(color_t(0.52, 0.36, 0.16));

    jupiter->setEphemeris(OrbitVSOP87::create(*jupiter, "vsop87b-jupiter"));
    jupiter->setMass(6.418542e+23);
    jupiter->setRadius(3389.5);
    jupiter->setAlbedo(0.250);
    jupiter->setColor(color_t(0.52, 0.36, 0.16));

    // saturn->setEphemeris(OrbitVSOP87::create(*saturn, "vsop87b-saturn"));
    // saturn->setMass(6.418542e+23);
    // saturn->setRadius(3389.5);
    // saturn->setAlbedo(0.250);
    // saturn->setColor(color_t(0.52, 0.36, 0.16));

    // uranus->setEphemeris(OrbitVSOP87::create(*uranus, "vsop87b-uranus"));
    // uranus->setMass(6.418542e+23);
    // uranus->setRadius(3389.5);
    // uranus->setAlbedo(0.250);
    // uranus->setColor(color_t(0.52, 0.36, 0.16));

    // neptune->setEphemeris(OrbitVSOP87::create(*neptune, "vsop87b-neptune"));
    // neptune->setMass(6.418542e+23);
    // neptune->setRadius(3389.5);
    // neptune->setAlbedo(0.250);
    // neptune->setColor(color_t(0.52, 0.36, 0.16));

    // lunar->setOrbit(VSOP87Orbit::create("elp-mpp02-llr-lunar"));
    // lunar->setRotation(Rotation::create("iau-lunar"));
    lunar->setEphemeris(OrbitELP82::create(*lunar, "elp82b-lunar"));
    lunar->setMass(7.342e+22);
    lunar->setRadius(1738.14);
    lunar->setAlbedo(0.136);
    lunar->setColor(color_t(1.0, 0.945582, 0.865));

    psys->addPlanet(mercury, sun);
    // psys->addPlanet(venus, sun);
    psys->addPlanet(earth, sun);
    psys->addPlanet(lunar, earth);
    psys->addPlanet(mars, sun);
    psys->addPlanet(jupiter, sun);
    // psys->addPlanet(saturn, sun);
    // psys->addPlanet(uranus, sun);
    // psys->addPlanet(neptune, sun);

    // std::ifstream in("systems/Sol/Sol.cfg", std::ios::in);
    // System::loadSolarSystemObjects(in, *this, "systems");
    // in.close();
}

void Universe::start(const TimeDate &td)
{
    Camera *cam = ofsAppCore->getCamera();
    Player *player = ofsAppCore->getPlayer();

    // cam->setPosition({ 0, 0, -sun->getRadius() * 4.0 });
    // cam->update();
    // player->attach(sun, camTargetRelative);
    // player->look(sun);

    // cam->setPosition({ 0, 0, mercury->getRadius() * 4.0 });
    // cam->update();
    // player->attach(mercury, camTargetRelative);
    // player->look(mercury);

    // cam->setPosition({ 0, 0, lunar->getRadius() * 4.0 });
    // cam->update();
    // player->attach(lunar, camTargetRelative);
    // player->look(lunar);

    // cam->setPosition({ 0, 0, mars->getRadius() * 4.0 });
    // cam->update();
    // player->attach(mars, camTargetRelative);
    // player->look(mars);

    // cam->setPosition({ 0, 0, jupiter->getRadius() * 4.0 });
    // cam->update();
    // player->attach(jupiter, camTargetRelative);
    // player->look(jupiter);

    cam->setPosition(earth->convertEquatorialToLocal(
        glm::radians(28.632307), glm::radians(-80.705774), earth->getRadius()+50));
    // cam->setPosition({ 0, 0, earth->getRadius() * 4.0 });
    cam->update();
    player->attach(earth, camTargetRelative);
    player->look(earth);

    // On Runway 15 (Cape Kennedy) - 28.632307, -80.705774
    // player->setGroundObserver(earth, { 28.632307, -80.705774, .002}, 150);

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