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
#include "render/vobject.h"
#include "render/scene.h"

void Scene::init()
{
    initStarRenderer();
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
    prm.cpos   = cam->getuPosition();
    prm.crot   = cam->getuOrientation();
    prm.dmProj = glm::perspective(cam->getFOV(), cam->getAspect(), 1.0, 1'000'000'000.0);
    prm.dmView = glm::transpose(glm::toMat4(prm.crot));

    // Render visible stars in background
    renderStars(universe.getStarDatabase(), player, faintestNightMag);

    // Render planetary systems with with suns (close stars)
    for (int idx = 0; idx < closeStars.size(); idx++)
    {
        const celStar *sun = closeStars[idx];

    }

    ctx.finish();
}