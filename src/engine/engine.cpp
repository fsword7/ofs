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

    player->start(realTime);

    celStar *sun   = universe->findStar("Sol");
    Object *planet = universe->findPath("Sol/Earth");
    Object *lunar  = universe->findPath("Sol/Earth/Moon");

    if (planet == nullptr)
    {
        fmt::printf("Planet is not found in universe!\n");
        return;
    }
    
    // player->move(planet, planet->getRadius() * 6.0, Player::goBackHelioSync);
    // player->follow(planet, Player::fwHelioSync);
    player->move(planet, planet->getRadius() * 6.0, Player::goGeoSync);
    player->follow(planet, Player::fwGeoSync);
    player->look(planet);

    // player->move(lunar, lunar->getRadius() * 6.0, Player::goGeoSync);
    // player->follow(lunar, Player::fwGeoSync);
    // player->look(lunar);

    // player->move(sun, sun->getRadius() * 6.0, Player::goEcliptic);
}

void Engine::update(double dt)
{
    player->update(dt, scaleTime);
}

void Engine::render()
{
    scene->render(*universe, *player);
}