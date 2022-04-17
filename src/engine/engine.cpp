// engine.cpp - OFS engine package
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#include "main/core.h"
#include "engine/engine.h"
#include "render/scene.h"

void Engine::init(Context *ctx, int width, int height)
{
    scene = new Scene(*ctx);
    scene->init();
}

void Engine::update(double dt)
{

}

void Engine::render()
{
    scene->render();
}