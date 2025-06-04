// psystem.cpp - Planetary System package
//
// Author:  Tim Stark
// Date:    Sep 17, 2023

#include "main/core.h"
#include "universe/universe.h"
#include "universe/celbody.h"
#include "universe/astro.h"
#include "universe/body.h"
#include "universe/star.h"
#include "engine/vehicle/vehicle.h"
#include "universe/psystem.h"

#include "yaml-cpp/yaml.h"

pSystem::pSystem(str_t &name)
: sysName(name)
{
}

pSystem::pSystem(Celestial *star)
: sysName(star->getsName()), primaryStar(star)
{
    star->setSystem(this);
    stars.push_back(star);
    bodies.push_back(star);
}

void pSystem::addStar(Celestial *star)
{
    if (primaryStar == nullptr)
        primaryStar = star;
    if (!star->hasSystem())
        star->setSystem(this);
    stars.push_back(star);
    // bodies.push_back(star);
    addBody(star);
    addCelestial(star);
}

void pSystem::addBody(Celestial *cbody)
{
    bodies.push_back(cbody);
}

void pSystem::addVehicle(Vehicle *vehicle)
{
    vehicles.push_back(vehicle);
    addBody(vehicle);
}

void pSystem::addCelestial(Celestial *cel)
{
    celestials.push_back(cel);

    std::sort(celestials.begin(), celestials.end(),
        [](Celestial *a, Celestial *b) { return a->getMass() > b->getMass(); } );
}

void pSystem::sortCelestials()
{
    std::sort(celestials.begin(), celestials.end(),
        [](Celestial *a, Celestial *b) { return a->getMass() > b->getMass(); } );
}

void pSystem::addPlanet(CelestialBody *planet, CelestialBody *parent)
{
    planets.push_back(planet);
    addBody(planet);
    addCelestial(planet);
    planet->setStar(primaryStar);
    planet->setSystem(this);
    planet->attach(parent);
}

bool pSystem::removeVehicle(Vehicle *vehicle)
{
    return false;
}

Vehicle *pSystem::getVehicle(cstr_t &name, bool incase) const
{
    for (auto vehicle : vehicles)
        if (vehicle->getName() == name)
            return vehicle;
    return nullptr;
}

Celestial *pSystem::find(cstr_t &name) const
{
    for (auto body : bodies)
        if (body->getsName() == name)
            return body;
    return nullptr;
}

glm::dvec3 pSystem::addSingleGravityPerturbation(const glm::dvec3 &rpos, const Celestial *body) const
{
    return { 0, 0, 0 };
}

// Calculate data with individual gravitational pull. 
glm::dvec3 pSystem::addSingleGravity(const glm::dvec3 &rpos, const Celestial *body) const
{
    double d = glm::length(rpos);
    return rpos * (astro::G * body->getMass() / (d*d*d)) + addSingleGravityPerturbation(rpos, body);
}

// Calculate data with N-body gravitational pull from entire system.
glm::dvec3 pSystem::addGravity(const glm::dvec3 &gpos, const Celestial *exclude) const
{
    glm::dvec3 acc = {};

    for (auto cel : celestials)
    {
        if (cel == exclude)
            continue;
        acc += addSingleGravity(cel->s0.pos - gpos, cel);
    }

    return acc;
}

// Calculate data with N-body gravitational pull from entire system.
glm::dvec3 pSystem::addGravityIntermediate(const glm::dvec3 &gpos, double step, const Celestial *exclude) const
{
    glm::dvec3 acc = {};

    for (auto cel : celestials)
    {
        if (cel == exclude)
            continue;
        // acc += addSingleGravity(cel->s0.pos - gpos, cel);
        acc += addSingleGravity(cel->interpolatePosition(step) - gpos, cel);
    }

    return acc;
}

void pSystem::update(bool force)
{

    for (auto star : stars)
        star->updateEphemeris();
    for (auto star : stars)
        star->updatePostEphemeris();
    for (auto cel : celestials)
        cel->updateCelestial(force);
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

bool pSystem::loadStar(const cstr_t &cbName, Universe *universe, pSystem *psys, cstr_t &cbPath)
{
    ofsLogger->info("Loading {}...\n", cbName);
    str_t fileName = cbName + ".yaml";

    YAML::Node config;
    config = YAML::LoadFile(cbPath + fileName);

    if (config["Activate"].IsScalar())
    {
        if (config["Activate"].as<bool>() == false)
        {
            ofsLogger->info("OFS Info: Disabled celestial body: {}\n",
                cbName);
            return false;
        }
    }

    str_t sysName;
    if (config["System"].IsScalar())
    {
        sysName = config["System"].as<std::string>();
        if (sysName.empty())
        {
            ofsLogger->info("OFS: system name not found in {} - aborted", fileName);
            return false;
        }
    }

    StarDatabase &stardb = universe->getStarDatabase();
    CelestialStar *star = stardb.find(sysName);
    if (star == nullptr)
    {
        ofsLogger->info("OFS: {} system not found\n", sysName);
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
    star->setPath(cbPath);
    // star->setFolder(cbFolder);
    psys->addStar(star);

    return true;
}

bool pSystem::loadPlanet(const cstr_t &cbName, pSystem *psys, cstr_t &cbPath)
{
    ofsLogger->info("Loading {}...\n", cbName);
    str_t fileName = cbName + ".yaml";

    YAML::Node config;
    config = YAML::LoadFile(cbPath + fileName);

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
        parent = dynamic_cast<CelestialBody *>(psys->find(sysName));
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

    cbody->setPath(cbPath);
    // cbody->setFolder(cbFolder);
    psys->addPlanet(cbody, parent);

    return true;
}

bool pSystem::loadSystems(Universe *universe, cstr_t &sysName)
{
    ofsLogger->info("Loading {} system...\n", sysName);

    str_t sysPath = OFS_HOME_DIR;
    sysPath += "/systems/" + sysName + "/";
    str_t sysFolder = sysName;
    str_t fileName = "system.yaml";

    YAML::Node config;

    try {
        config = YAML::LoadFile(sysPath + fileName);
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
    psys->sysPath = sysPath;
    psys->sysFolder = sysFolder;
    // pSystem *psys = new pSystem(sun);
    ofsLogger->info("Creating {} planetary system...\n", name);

    for (auto body : config["stars"])
    {
        // ofsLogger->info("Star: {}\n", planet.as<std::string>());
        str_t cbName = body.as<std::string>();
        str_t cbPath = sysPath + cbName + "/";
        str_t cbFolder = sysFolder + "/" + cbName;
        loadStar(cbName, universe, psys, cbPath);
    }

    for (auto body : config["planets"])
    {
        // ofsLogger->info("Planet: {}\n", planet.as<std::string>());
        str_t cbName = body.as<std::string>();
        str_t cbPath = sysPath + cbName + "/";
        str_t cbFolder = sysFolder + "/" + cbName;
        loadPlanet(cbName, psys, cbPath);
    }

    return true;
}