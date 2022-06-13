// body.cpp - Celestial Body package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "engine/rigidbody.h"
#include "ephem/rotation.h"
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
    return quatd_t::Identity();
}

double celBody::getLuminosity(double lum, double dist) const
{
    double power = lum * SOLAR_POWER;
    double irradiance = power / ofs::sphereArea(dist * 1000.0);
    double incidentEnergy = irradiance * ofs::circleArea(radius * 1000.0);
    double reflectedEnergy = incidentEnergy * albedo;

    return reflectedEnergy / SOLAR_POWER;
}

double celBody::getApparentMagnitude(vec3d_t sun, double irradiance, vec3d_t view) const
{
    double vdist  = view.norm();
    double sdist  = sun.norm();
    double illum  = (1.0 * (sun / sdist).dot(sun / sdist)) / 2.0;
    double absMag = getLuminosity(irradiance, sdist) * illum;

    return astro::convertLumToAppMag(absMag, astro::convertKilometerToParsec(vdist));
}

quatd_t celBody::getBodyFixedFromEcliptic(double tjd) const
{
    return (rotation != nullptr ? rotation->getRotation(tjd) : quatd_t::Identity()) * bodyFrame->getOrientation(tjd);
}

vec3d_t celBody::getPlanetocentric(vec3d_t pos) const
{
    vec3d_t w = pos.normalized();

    double lat = acos(w.y()) - (pi / 2.0);
    double lng = atan2(w.z(), -w.x());

    return vec3d_t(lat, lng, pos.norm() - radius);
}

vec3d_t celBody::getPlanetocentricFromEcliptic(const vec3d_t &pos, double tjd) const
{
    vec3d_t lpos = getBodyFixedFromEcliptic(tjd) * pos;

    return getPlanetocentric(lpos);
}

vec3d_t celBody::getvPlanetocentricFromEcliptic(const vec3d_t &pos, double tjd) const
{
    return getBodyFixedFromEcliptic(tjd) * pos;
}

vec3d_t celBody::getHeliocentric(double tjd) const
{
    vec3d_t opos = getuPosition(tjd);
    vec3d_t hpos = opos.normalized();

    double lat = acos(hpos.y()) - (pi / 2.0);
    double lng = atan2(hpos.z(), -hpos.x());

    return vec3d_t( lng, lat, opos.norm());
}