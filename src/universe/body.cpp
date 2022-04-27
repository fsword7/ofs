// body.cpp - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "universe/body.h"
#include "universe/astro.h"

// ******** Planetary System ********

PlanetarySystem::PlanetarySystem(celBody *body)
: body(body), tree(body)
{
    bodies.clear();
}

PlanetarySystem::PlanetarySystem(celStar *star)
: star(star), tree(star)
{
    bodies.clear();
}

void PlanetarySystem::addBody(celBody *body)
{
    bodies.push_back(body);
    body->setInSystem(this);
}

void PlanetarySystem::removeBody(celBody *body)
{
}

celBody *PlanetarySystem::find(cstr_t &name) const
{
    for (auto body : bodies)
    {
        if (name == body->getsName())
            return body;
    }
    return nullptr;
}

// ******** Celestial Body ********

PlanetarySystem *celBody::createPlanetarySystem()
{
    if (ownSystem != nullptr)
        return ownSystem;

    ownSystem = new PlanetarySystem(this);
    ownSystem->setStar(inSystem->getStar());
    return ownSystem;
}

quatd_t celBody::getEquatorial(double tjd) const
{
    return { 1, 0, 0, 0 };
}

double celBody::getLuminosity(double lum, double dist) const
{
    double power = lum * SOLAR_POWER;
    double irradiance = power / sphereArea(dist * 1000.0);
    double incidentEnergy = irradiance * circleArea(radius * 1000.0);
    double reflectedEnergy = incidentEnergy * albedo;

    return reflectedEnergy / SOLAR_POWER;
}

double celBody::getApparentMagnitude(vec3d_t sun, double irradiance, vec3d_t view) const
{
    double vdist  = glm::length(view);
    double sdist  = glm::length(sun);
    double illum  = (1.0 * glm::dot(view / vdist, sun / sdist)) / 2.0;
    double absMag = getLuminosity(irradiance, sdist) * illum;

    return astro::convertLumToAppMag(absMag, astro::convertKilometerToParsec(vdist));
}

vec3d_t celBody::getPlanetocentric(vec3d_t pos)
{
    vec3d_t w = glm::normalize(pos);

    double lat = acos(w.y) - (pi / 2.0);
    double lng = atan2(w.z, -w.x);

    return vec3d_t(lng, lat, glm::length(pos) - radius);
}