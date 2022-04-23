// scene.cpp - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/shader.h"
#include "engine/player.h"
#include "universe/star.h"
#include "universe/universe.h"
#include "render/scene.h"
#include "render/vobject.h"

void Scene::init(Universe &universe)
{
    initStarRenderer();
    initConstellations(universe);
}

vec3d_t Scene::getAstrocentericPosition(const celStar *sun, vec3d_t upos, double now)
{
    return upos - sun->getuPosition(now);
}

void Scene::render(Universe &universe, Player &player)
{

    closeStars.clear();

    // find closest stars with player's position
    vec3d_t obs = player.getuPosition();
    universe.findCloseStars(obs, 1.0, closeStars);

    ctx.start();

    Camera *cam = player.getCamera();
    pixelSize = (2.0 * tan(cam->getFOV() / 2.0)) / cam->getHeight();

    // Projection matrix in universal frame
    // for rendering stars and constellations
    prm.jnow   = player.getJulianTime();
    prm.cpos   = cam->getuPosition();
    prm.crot   = cam->getuOrientation();
    prm.dmProj = glm::perspective(cam->getFOV(), cam->getAspect(), 1.0, 1'000'000'000.0);
    prm.dmView = glm::transpose(glm::toMat4(prm.crot));

    // Render constellations in background
    renderConstellations(universe, player);

    // Render visible stars in background
    renderStars(universe.getStarDatabase(), player, faintestNightMag);

    // Render planetary systems with with suns (close stars)
    for (int idx = 0; idx < closeStars.size(); idx++)
    {
        const celStar *sun = closeStars[idx];

        if (!sun->hasSolarSytstem())
            continue;

        System *system = sun->getSolarSystem();
        PlanetarySystem *objects = system->getPlanetarySystem();
        FrameTree *tree = objects->getSystemTree();

        vec3d_t apos = getAstrocentericPosition(sun, obs, prm.jnow);
        vec3d_t vpn = glm::conjugate(prm.crot) * vec3d_t (0, 0, -1);

        buildNearSystems(tree, player, apos, vpn, { 0, 0, 0 });
    }

    ctx.finish();
}