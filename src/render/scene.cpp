// scene.cpp - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "engine/player.h"
#include "universe/star.h"
#include "universe/universe.h"
#include "render/vobject.h"
#include "render/scene.h"

void Scene::init()
{

}

void Scene::render(Universe &universe, Player &player)
{

    closeStars.clear();

    // find closest stars with player's position
    vec3d_t obs = player.getuPosition();
    universe.findCloseStars(obs, 1.0, closeStars);

    ctx.start();

    // Render planetary systems with with suns (close stars)
    for (int idx = 0; idx < closeStars.size(); idx++)
    {
        const celStar *sun = closeStars[idx];

    }

    ctx.finish();
}