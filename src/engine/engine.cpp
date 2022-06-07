// engine.cpp - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/buffers.h"
#include "osd/gl/shader.h"
#include "engine/engine.h"
#include "engine/object.h"
#include "engine/player.h"
#include "universe/universe.h"
#include "universe/star.h"
#include "render/gl/fonts.h"
#include "render/scene.h"
#include "render/overlay.h"

void Engine::init(Context *ctx, int width, int height)
{
    scene = new Scene(*ctx);
    universe = new Universe();
    universe->init();
    scene->init(*universe);

    player = new Player();
    player->getCamera()->setViewport(width, height);

    overlay = new Overlay(*ctx);
    titleFont = TextureFont::load(*ctx, "fonts/OpenSans-Bold.ttf", 20);
    textFont = TextureFont::load(*ctx, "fonts/OpenSans-Regular.ttf", 12);
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

    // player->move(planet, planet->getRadius() * 6.0, Player::goFrontHelioSync);
    // player->follow(planet, Player::fwHelioSync);
    // player->move(planet, planet->getRadius() * 6.0, Player::goBackHelioSync);
    // player->follow(planet, Player::fwHelioSync);
    // player->move(planet, planet->getRadius() * 6.0, Player::goBodyFixed);
    // player->follow(planet, Player::fwBodyFixed);
    // player->move(planet, planet->getRadius() * 6.0, Player::goEquartorial);   
    // player->follow(planet, Player::fwEquatorial);
    player->move(planet, planet->getRadius() * 6.0, Player::goEcliptic);
    player->follow(planet, Player::fwEcliptic);
    player->look(planet);

    // player->move(lunar, lunar->getRadius() * 6.0, Player::goBodyFixed);
    // player->follow(lunar, Player::fwBodyFixed);
    // player->look(lunar);

    // player->move(sun, sun->getRadius() * 6.0, Player::goEcliptic);
    // player->look(sun);

    focusObject = planet;
}

void Engine::update(double dt)
{
    player->update(dt, scaleTime);
}

void Engine::render()
{
    scene->render(*universe, *player);
    renderOverlay();
}

Object *Engine::pickObject(const vec3d_t &pickRay)
{
    return universe->pick(player->getuPosition(),
        glm::conjugate(player->getuOrientation()) * pickRay,
        player->getJulianTime());
}