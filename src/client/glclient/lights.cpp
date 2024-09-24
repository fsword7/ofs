// lights.cpp - Light source routine package
//
// Author:  Tim Stark
// Date:    Oct 22, 2023

#include "main/core.h"
#include "universe/astro.h"
#include "universe/star.h"
#include "client.h"
#include "starcolors.h"
#include "scene.h"
#include "lights.h"
// #include "universe/body.h"

void Scene::setupPrimaryLightSources(const std::vector<const CelestialStar *> nearStars,
    const glm::dvec3 &obs, std::vector<LightSource> &ls)
{
    for (auto star : nearStars)
    {
        LightSource ls;

        ls.spos       = star->getoPosition() - obs;
        ls.luminosity = star->getLuminosity();
        ls.radius     = star->getRadius();
        ls.color      = starColors->lookup(star->getTemperature());

        lightSources.push_back(ls);
    }
}

void Scene::setupSecondaryLightSources()
{
}

void Scene::setObjectLighting(std::vector<LightSource> &suns, const glm::dvec3 &opos,
    const glm::dquat &orot, LightState &ls)
{
    uint32_t nLights = std::min((uint32_t)MAX_LIGHTS, (uint32_t)suns.size());
    if (nLights == 0)
        return;
    
    for (int idx = 0; idx < nLights; idx++)
    {
        glm::dvec3 spos = opos - suns[idx].spos;
        double     dist = glm::length(spos);
        double     au   = astro::convertKilometerToAU(dist);

        ls.lights[idx].spos       = spos;
        ls.lights[idx].asize      = (suns[idx].radius / dist);
        ls.lights[idx].color      = suns[idx].color;
        ls.lights[idx].irradiance = suns[idx].luminosity / (au * au);
        ls.lights[idx].dEye       = spos * (1.0 / dist);
        // ls.lights[idx].dObject    = ls.lights[idx].dEye * orot;
        ls.lights[idx].shadows    = true;
    }


    // Sorting light sources by brightness
    if (nLights > 1)
        std::sort(ls.lights, ls.lights + nLights,
            [](const auto &l0, const auto &l1)
            { return l0.irradiance > l1.irradiance; }
            );

    // Calculating total irradiance from multi-star systems
    double totalIrradiance = 0.0;
    for (int idx = 0; idx < nLights; idx++)
        totalIrradiance += ls.lights[idx].irradiance;

    // float visibleFraction = 1.0f / 10000.0f;
    // float displayable = 1.0f / 255.0f;
    // float gamma = log(displayable) / log(visibleFraction);
    // float visibleIrradiance = visibleFraction * totalIrradiance;

    ls.nLights = nLights;
    for (int idx = 0; idx < nLights; idx++)
        ls.lights[idx].irradiance /= totalIrradiance;
    ls.ambientColor = { 0, 0, 0 };

    // for (int idx = 0; idx < nLights; idx++)
    // {
    //     // ls.lights[idx].irradiance = pow(ls.lights[idx].irradiance / totalIrradiance, gamma);
    //     ls.lights[idx].irradiance /= totalIrradiance;
    //     ls.nLights++;
    // }

    // ls.eyePosObject = -opos * orot;
    // ls.eyeDirObject = (vec3d_t(0, 0, 0) - opos) * orot;
    // ls.ambientColor = vec3d_t(0, 0, 0);

}