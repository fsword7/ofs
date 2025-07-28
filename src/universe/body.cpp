// body.cpp - Celestial body (Planets, Moons, Asteroids, etc) package
//
//  Author: Tim Stark
//  Date:   Sep 17, 2023

#include "main/core.h"
#include "engine/base.h"
#include "universe/body.h"
#include "utils/json.h"

CelestialPlanet::CelestialPlanet(cstr_t &name, celType type)
: CelestialBody(name, objCelestialBody, type)
{
    emgr = new ElevationManager(this);
}

CelestialPlanet::CelestialPlanet(json &config, celType type)
: CelestialBody(config, objCelestialBody, type)
{
    str_t atmName = myjson::getString<str_t>(config, "atmosphere");
    if (!atmName.empty()) {
        Atmosphere *atmp = Atmosphere::create(atmName);
        if (atmp != nullptr) {
            atm = atmp;
            atm->getAtmConstants(atmc);
        } else
            ofsLogger->error("OFS: Unknown atmosphere model: {}\n", atmName);
    }

    atmc.color0 = myjson::getFloatArray<color_t, float>(config, "atm-color", { 0, 0, 0 });

    if (config["ground-observers"].is_array()) {

    }

    enableSecondaryIlluminator(true);

    // Initialize configurations from Yaml database.
    setup();
}

// CelestialPlanet::CelestialPlanet(YAML::Node &config, celType type)
// : CelestialBody(config, objCelestialBody, type)
// {
//     str_t atmName = yaml::getValueString<str_t>(config, "Atmosphere");
//     if (!atmName.empty())
//     {
//         Atmosphere *atmp = Atmosphere::create(atmName);
//         if (atmp != nullptr) {
//             atm = atmp;
//             atm->getAtmConstants(atmc);
//         } else
//             ofsLogger->error("OFS Error: Unknown atmosphere model: {}\n", atmName);
//     }

//     atmc.color0 = yaml::getArray<color_t, float>(config, "AtmColor", {0, 0, 0});

//     if (config["GroundObservers"].IsSequence())
//     {
//         const YAML::Node &poiList = config["GroundObservers"];
//         str_t poiName;
//         glm::dvec3 gloc;
//         double gdir;
//         ofsLogger->info("Ground Observers: ({} records)\n", poiList.size());
//         for (int idx = 0; idx < poiList.size(); idx++) {
//             const YAML::Node &poi = poiList[idx];
//             if (poi.IsSequence())
//             {
//                 poiName = poi[0].as<str_t>();
//                 if (poi[1].IsSequence() && poi[1].size() == 3) {
//                     gloc.x = poi[1][0].as<double>();
//                     gloc.y = poi[1][1].as<double>();
//                     gloc.z = poi[1][2].as<double>();
//                 }
//                 gdir = poi[2].as<double>();

//                 addGroundPOI(poiName, gloc, gdir);
//                 ofsLogger->info("POI: {}, ({}, {}, {}) heading {}\n",
//                     poiName, gloc.x, gloc.y, gloc.z, gdir);
//             }
//         }
//     }

//     enableSecondaryIlluminator(true);

//     // Initialize configurations from Yaml database.
//     setup();
// }

CelestialPlanet::~CelestialPlanet()
{
    if (emgr != nullptr)
        delete emgr;

    for (auto &base : baseList)
        delete base;
    baseList.clear();
}

void CelestialPlanet::setup()
{

    // Initialize surface bases. 
    emgr = new ElevationManager(this);
    for (auto &base : baseList)
        base->setup();
}

void CelestialPlanet::getAtmParam(const glm::dvec3 &loc, atmprm_t *prm) const
{
    // Clear all atomspheric parameters
    *prm = {};

    if (atm != nullptr)
    {
        iatmprm_t in;

        in.lat = loc.x;
        in.lng = loc.y;
        in.alt = loc.z;
    
        atm->getAtmParams(in, *prm);
    }
}

double CelestialPlanet::getElevation(glm::dvec3 ploc)
{
    return (emgr != nullptr) ? emgr->getElevationData(ploc) : 0;
}

glm::dvec3 CelestialPlanet::getGroundVelocity(const glm::dvec3 &ploc)
{
    double lat = ofs::radians(ploc.x), lng = ofs::radians(ploc.y);
    double vel = (getRadius()+ploc.z) * cos(lat) / getRotationPeriod();

    return {-vel*sin(lng), 0.0, -vel*cos(lng) };
}

glm::dvec3 CelestialPlanet::getWindVelocity(const glm::dvec3 &ploc, double alt)
{
    return {0, 0, 0};
}

void CelestialPlanet::addGroundPOI(cstr_t &name, glm::dvec3 &loc, double dir)
{
    // Adding site ground observer/point of interest
    GroundPOI *poi = new GroundPOI(name, loc, dir);

    poiList.push_back(poi);
}

bool CelestialPlanet::addBase(Base *base)
{
    if (getBase(base->getsName()))
        return false;
    base->attach(this);
    baseList.push_back(base);

    return true;
}

Base *CelestialPlanet::getBase(cstr_t &name)
{
    for (auto &base : baseList)
        if (name == base->getsName())
            return base;
    return nullptr;
}