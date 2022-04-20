// engine.cpp - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "engine/engine.h"
#include "engine/player.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "render/scene.h"

void Engine::init(Context *ctx, int width, int height)
{
    scene = new Scene(*ctx);
    scene->init();

    universe = new Universe();
    universe->init();

    player = new Player();
    player->getCamera()->setViewport(width, height);
}

void Engine::start()
{
    // Start real time with julian date/time.
    realDate.reset();
    realTime  = realDate;
    scaleTime = 1.0;

    celStar *sun = universe->findStar("Sol");

    player->move(sun, sun->getRadius() * 6.0, Player::goEcliptic);
}

void Engine::update(double dt)
{
    player->update(dt, scaleTime);
}

void Engine::render()
{
    scene->render(*universe, *player);
}