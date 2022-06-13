// lights.cpp - Light source routine package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "osd/gl/lights.h"
#include "universe/astro.h"
#include "universe/body.h"
#include "universe/star.h"
#include "render/scene.h"

void Scene::setupPrimaryLightSources(const std::vector<const celStar *> nearStars,
    const vec3d_t &obs, double now, std::vector<LightSource> &ls)
{
    for (int idx = 0; idx < nearStars.size(); idx++)
    {
        const celStar *star = nearStars[idx];
        LightSource ls;

        ls.spos       = star->getStarPosition() - (obs / KM_PER_PC);
        ls.luminosity = star->getLuminosity();
        ls.radius     = star->getRadius();
        // ls.color     = starColors->lookup(star->getTemperature());

        lightSources.push_back(ls);
    }
}

void Scene::setupSecondaryLightSources()
{
}

void Scene::setObjectLighting(std::vector<LightSource> &suns, const vec3d_t opos,
    const quatd_t orot, LightState &ls)
{
    uint32_t nLights = std::min((uint32_t)MAX_LIGHTS, (uint32_t)suns.size());
    if (nLights == 0)
        return;
    
    for (int idx = 0; idx < nLights; idx++)
    {
        vec3d_t spos = opos - suns[idx].spos;
        double  dist = spos.norm();
        double  au   = astro::convertKilometerToAU(dist);

        ls.lights[idx].spos       = spos;
        ls.lights[idx].asize      = (suns[idx].radius / dist);
        ls.lights[idx].color      = suns[idx].color;
        ls.lights[idx].irradiance = suns[idx].luminosity / (au * au);
        ls.lights[idx].dEye       = spos * (1.0 / dist);
        // ls.lights[idx].dObject    = ls.lights[idx].dEye * orot;
        ls.lights[idx].shadows    = true;
    }

    if (nLights > 1)
    {
        // Calculating total irradiance from multi-star systems
        double totalIrradiance = 0.0;
        for (int idx = 0; idx < nLights; idx++)
            totalIrradiance += ls.lights[idx].irradiance;

        // Determine first brightest light sources
        // sort(ls.lights, ls.lights + nLights, LightIrradiancePredicate());
    }

    // ls.eyePosObject = -opos * orot;
    // ls.eyeDirObject = (vec3d_t(0, 0, 0) - opos) * orot;
    // ls.ambientColor = vec3d_t(0, 0, 0);
}