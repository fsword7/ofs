// scene.cpp - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/mesh.h"
#include "osd/gl/shader.h"
#include "engine/player.h"
#include "universe/star.h"
#include "universe/universe.h"
#include "render/gl/fonts.h"
#include "render/scene.h"
#include "render/vobject.h"
#include "render/surface.h"
#include "main/debug.h"

void Scene::init(Universe &universe)
{
    // Initialize global parameters
    SurfaceManager::ginit();
    TextureFont::ginit();

    initStarRenderer();
    initConstellations(universe);
}

void Scene::cleanup()
{
    SurfaceManager::gexit();
    TextureFont::gexit();
}

vec3d_t Scene::getAstrocentericPosition(const celStar *sun, vec3d_t upos, double now)
{
    return upos - sun->getuPosition(now);
}

void Scene::render(Universe &universe, Player &player)
{

    closeStars.clear();
    lightSources.clear();
    objectList.clear();

    // Clear all annotation lists
    bgAnnotations.clear();
    fgAnnotations.clear();
    dsAnnotations.clear();
    objAnnotations.clear();

    // find closest stars with player's position
    vec3d_t obs = player.getuPosition();
    prm.jnow    = player.getJulianTime();
    // universe.findCloseStars(obs, 1.0, closeStars);
    setupPrimaryLightSources(closeStars, obs, prm.jnow, lightSources);

    ctx.start();

    Camera *cam = player.getCamera();
    pixelSize = (2.0 * tan(cam->getFOV() / 2.0)) / cam->getHeight();

    // Projection matrix in universal frame
    // for rendering stars and constellations
    prm.cpos   = cam->getuPosition();
    prm.crot   = cam->getuOrientation();
    prm.tanap  = cam->getTanAp();
    prm.pxSize = pixelSize;
    prm.dmProj = ofs::perspective(cam->getFOV(), cam->getAspect(), 0.0001, 1'000'000'000.0);
    prm.dmView = Eigen::Affine3d(prm.crot).matrix();

    // Render constellations in background
    renderConstellations(universe, player);

    // Render visible stars in background
    renderStars(universe.getStarDatabase(), player, faintestNightMag);

    // Render planetary systems with with suns (close stars)
    for (int idx = 0; idx < closeStars.size(); idx++)
    {
        const celStar *sun = closeStars[idx];

        if (!sun->hasSolarSystem())
            continue;

        System *system = sun->getSolarSystem();
        PlanetarySystem *objects = system->getPlanetarySystem();
        FrameTree *tree = objects->getSystemTree();

        vec3d_t apos = getAstrocentericPosition(sun, obs, prm.jnow);
        vec3d_t vpn = prm.crot.conjugate() * vec3d_t (0, 0, -1);

        buildNearSystems(tree, player, apos, vpn, { 0, 0, 0 });
    }

    ctx.finish();
}