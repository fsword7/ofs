// universe.cpp - Universe package
//
// Author:  Tim Stark
// Date:    Apr 18, 2022

#include "main/core.h"
#include "ephem/orbit.h"
#include "ephem/vsop87.h"
#include "ephem/rotation.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/system.h"
#include "universe/astro.h"
#include "scripts/parser.h"

#include "main/app.h"
#include "engine/camera.h"

void Universe::init()
{
    stardb.loadXHIPData("data/xhip");
    constellations.load("data/constellations/western/constellationship.fab");
    // constellations.load("constellations/western_rey/constellationship.fab");

    celStar *sun = stardb.find("Sol");
    System *solSystem = createSolarSystem(sun);
    PlanetarySystem *system = solSystem->getPlanetarySystem();

    celBody *mercury, *venus, *earth;
    celBody *mars, *jupiter, *saturn;
    celBody *uranus, *neptune, *lunar;

    // mercury = System::createBody("Mercury", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    // venus = System::createBody("Venus", system, celType::cbPlanet,
    //     "EclipticJ2000", "EquatorJ2000");
    earth = System::createBody("Earth", system, celType::cbPlanet,
        "EclipticJ2000", "EclipticJ2000");
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

    earth->setColor(color_t(0.856, 0.910, 1.0));
    earth->setOrbit(VSOP87Orbit::create("vsop87-earth"));
    earth->setRotation(Rotation::create("p03lp-earth"));
    // earth->setRotation(Rotation::create("iau-earth"));
    earth->setRadius(6378.140);
    earth->setAlbedo(0.449576);

    // lunar->setColor(color_t(1.0, 0.945582, 0.865));
    // lunar->setOrbit(VSOP87Orbit::create("elp-mpp02-llr-lunar"));
    // lunar->setRotation(Rotation::create("iau-lunar"));
    // lunar->setRadius(1738.14);
    // lunar->setAlbedo(0.136);

    Camerax *cam = ofsAppCore->getCamera();
    cam->setPosition(glm::dvec3(0, 0, -sun->getRadius() * 4.0));
    cam->look(sun->getStarPosition());
    cam->update();

    // cam->setPosition(glm::dvec3(0, 0, -earth->getRadius() * 4.0));
    // cam->look(glm::dvec3(0, 0, 0));
    // cam->update();

    std::ifstream in("systems/Sol/Sol.cfg", std::ios::in);
    System::loadSolarSystemObjects(in, *this, "systems");
    in.close();
}

System *Universe::createSolarSystem(celStar *star)
{
    System *system = getSolarSystem(star);
    if (system != nullptr)
        return system;

    auto idStar = star->getHIPnumber();
    system = new System(star);
    systems.insert({idStar, system});

    return system;
}

System *Universe::getSolarSystem(celStar *star) const
{
    if (star == nullptr)
        return nullptr;
    auto idStar = star->getHIPnumber();
    auto iter = systems.find(idStar);
    if (iter != systems.end())
        return iter->second;
    return nullptr;
}

celStar *Universe::findStar(cstr_t &name) const
{
    return stardb.find(name);
}

Object *Universe::findObject(const Object *obj, cstr_t &name) const
{
    System *system;
    const celStar *sun;
    const celBody *body;
    const PlanetarySystem *objects;

    switch (obj->getType())
    {
    case ObjectType::objCelestialStar:
        sun = dynamic_cast<const celStar *>(obj);
        if ((system = sun->getSolarSystem()) == nullptr)
            break;
        objects = system->getPlanetarySystem();
        return objects->find(name);

    case ObjectType::objCelestialBody:
        body = dynamic_cast<const celBody *>(obj);
        objects = body->getOwnSystem();
        if (objects != nullptr)
            return objects->find(name);
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

// int Universe::findCloseStars(const vec3d_t &obs, double mdist,
//     std::vector<const celStar *> &closeStars) const
// {
//     return stardb.findCloseStars(obs, mdist, closeStars);
// }

int Universe::findCloseStars(const glm::dvec3 &obs, double mdist,
    std::vector<ObjectHandle> &closeStars) const
{
    return stardb.findCloseStars(obs, mdist, closeStars);
}

void Universe::findVisibleStars(ofsHandler2 &handler,
    const glm::dvec3 &obs, const glm::dmat3 &rot,
    const double fov, const double aspect,
    const double faintest)
{
    stardb.findVisibleStars(handler, obs, rot, fov, aspect, faintest);
}

Object *Universe::pickPlanet(System *system, const glm::dvec3 &obs, const glm::dvec3 &dir, double when)
{

    return nullptr;
}

Object *Universe::pick(const glm::dvec3 &obs, const glm::dvec3 &dir, double when)
{
    // std::vector<const celStar *> closeStars;
    // Object *picked = nullptr;

    // findCloseStars(obs, 1.0, closeStars);
    // for (auto star : closeStars)
    // {
    //     if (!star->hasSolarSystem())
    //         continue;
    //     System *system = star->getSolarSystem();
    //     picked = pickPlanet(system, obs, dir, when);
    // }

    // return picked;
    return nullptr;
}