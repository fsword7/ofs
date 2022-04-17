// app.cpp - Core application main routines
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "engine/engine.h"
#include "engine/player.h"
#include "render/scene.h"
#include "main/app.h"

CoreApp::CoreApp()
: width(SCR_WIDTH), height(SCR_HEIGHT)
{
    // Initialize state keys
    for (int idx = 0; idx < 512; idx++)
    {
        stateKey[idx] = false;
        shiftStateKey[idx] = false;
        ctrlStateKey[idx] = false;
        altStateKey[idx] = false;
    }
}

void CoreApp::initEngine()
{
    engine = new Engine();
    engine->init(ctx, width, height);

    scene = engine->getScene();
    player = engine->getPlayer();
    camera = player->getCamera();
}

void CoreApp::update()
{
}

void CoreApp::render()
{
    if (scene != nullptr)
        scene->render(*player);
}