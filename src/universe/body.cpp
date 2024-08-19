// body.cpp - Celestial body (Planets, Moons, Asteroids, etc) package
//
//  Author: Tim Stark
//  Date:   Sep 17, 2023

#include "main/core.h"
#include "universe/body.h"
#include "utils/yaml.h"

CelestialPlanet::CelestialPlanet(cstr_t &name, celType type)
: CelestialBody(name, objCelestialBody, type)
{
    emgr = new ElevationManager(this);
}

CelestialPlanet::CelestialPlanet(YAML::Node &config, celType type)
: CelestialBody(config, objCelestialBody, type)
{
    emgr = new ElevationManager(this);   
}

CelestialPlanet::~CelestialPlanet()
{
    if (emgr != nullptr)
        delete emgr;
}

void CelestialPlanet::setup(YAML::Node &config)
{

    // atmc.color0 = yaml::getValue<color_t>(config, "AtomsphereColor");

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