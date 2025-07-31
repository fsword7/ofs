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
#include "engine/vehicle/svehicle.h"
#include "engine/vehicle/vehicle.h"
#include "universe/psystem.h"

#include "utils/json.h"

pSystem::pSystem(cstr_t &name)
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

void pSystem::addSuperVehicle(SuperVehicle *svehicle)
{
    svehicles.push_back(svehicle);
    addBody(svehicle);
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

Vehicle *pSystem::findVehicle(cstr_t &name) const
{
    for (auto veh : vehicles)
        if (veh->getsName() == name)
            return veh;
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
    // Enable update states
    for (auto body : bodies)
        body->beginUpdate();

    // Updating solar/planetary system
    for (auto star : stars)
        star->updateEphemeris();
    for (auto star : stars)
        star->updatePostEphemeris();
    for (auto cel : celestials)
        cel->updateCelestial(force);

    // ofsLogger->info("{} system: {} super vehicles {} vehicles\n",
    //     primaryStar->getsName(), svehicles.size(), vehicles.size());

    // Updating vehicles within solar system
    for (auto veh : vehicles)
        veh->updateBodyForces();
    for (auto sveh : svehicles)
        sveh->update(force);
    for (auto veh : vehicles)
        veh->update(force);
}

void pSystem::finalizeUpdate()
{
    // Finalize vehicle updates
    for (auto sveh : svehicles)
        sveh->updatePost();
    for (auto veh : vehicles)
        veh->updatePost();

    // Finalize updates
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

bool pSystem::loadStar(cstr_t &cbName, Universe *univ, pSystem *psys, fs::path &cbPath)
{
    ofsLogger->info("Loading {}...\n", cbName);
    fs::path path = cbPath / "cbody.json";

    std::ifstream jsonFile(path);
    json config = json::parse(jsonFile, nullptr, false, true);

    if (!myjson::getBoolean<bool>(config, "activate", true)) {
        ofsLogger->info("OFS info: Disabled celestial body: {}\n", cbName);
        return false;
    }

    str_t sysName = myjson::getString<str_t>(config, "system");
    if (sysName.empty()) {
        ofsLogger->error("OFS: Required planetary system name - aborted\n");
        return false;
    }

    StarDatabase &stardb = univ->getStarDatabase();
    CelestialStar *star = stardb.find(sysName);
    if (star == nullptr) {
        ofsLogger->error("OFS: Unknown star system: {} - aborted\n", sysName);
        return false;
    }

    star->configure(config);
    star->setPath(cbPath);
    // star->setFolder(cbFolder);
    psys->addStar(star);

    return true;
}

bool pSystem::loadPlanet(cstr_t &cbName, pSystem *psys, fs::path &cbPath)
{
    ofsLogger->info("Loading {}...\n", cbName);
    fs::path path = cbPath / "cbody.json";

    std::ifstream jsonFile(path);
    json config = json::parse(jsonFile, nullptr, false, true);

    if (!myjson::getBoolean<bool>(config, "activate", true)) {
        ofsLogger->info("OFS info: Disabled celestial body: {}\n", cbName);
        return false;
    }

    CelestialBody *parent = nullptr;
    str_t sysName = myjson::getString<str_t>(config, "system");
    if (sysName.empty()) {
        ofsLogger->error("OFS: Required planetary system name - aborted\n");
        return false;
    }
    parent = dynamic_cast<CelestialBody *>(psys->find(sysName));
    if (parent == nullptr) {
        ofsLogger->error("OFS: Unknown planetary system: {} - aborted\n", sysName);
        return false;
    }

    celType type = cbUnknown;
    str_t typeName = myjson::getString<str_t>(config, "type");
    if (typeName.empty()) {
        ofsLogger->error("OFS: Required celestial body type - aborted\n");
        return false;
    }
    for (auto ctype : celTypes)
        if (ctype.name == typeName)
            type = ctype.type;
    if (type == cbUnknown)
    {
        ofsLogger->info("OFS: Unknown celestial body type: {} - aborted\n",
            typeName);
        return false;
    }

    CelestialPlanet *cbody = new CelestialPlanet(config, type);

    cbody->setPath(cbPath);
    // cbody->setFolder(cbFolder);
    psys->addPlanet(cbody, parent);

    return true;
}

bool pSystem::loadSystem(Universe *univ, cstr_t &sysName, const fs::path &path)
{
    ofsLogger->info("Loading {} system...\n", sysName);

    fs::path fname = path / "system.json";
    json sysConfig;
    // str_t sysName;

    std::ifstream jsonFile(fname);
    sysConfig = json::parse(jsonFile, nullptr, false, true);

    // pSystem *psys = new pSystem(sysName);
    pSystem *psys = univ->createSolarSystem(sysName);
    ofsLogger->info("Creating {} planetary system...\n", sysName);

    if (sysConfig["celestial-bodies"].is_array())
    {
        for (auto &item : sysConfig["celestial-bodies"].items())
        {
            if (!item.value().is_object())
                continue;
            auto &entry = item.value();
            str_t cbName, cbType;
            fs::path cbPath = OFS_HOME_DIR;
            cbPath /= "systems";

            if (entry["name"].is_string())
                cbName = entry["name"].get<str_t>();
            if (entry["type"].is_string())
                cbType = entry["type"].get<str_t>();
            if (entry["folder"].is_string())
                cbPath /= entry["folder"].get<fs::path>();

            if (cbType == "star")
                loadStar(cbName, univ, psys, cbPath);
            else
                loadPlanet(cbName, psys, cbPath);
        }
    }

    return true;
}
