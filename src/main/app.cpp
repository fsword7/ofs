// app.cpp - Core application main routines
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#include "main/core.h"
#include "engine/engine.h"
#include "engine/player.h"
#include "universe/universe.h"
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
    universe = engine->getUniverse();
    player = engine->getPlayer();
    camera = player->getCamera();
}

void CoreApp::start()
{
    if (engine != nullptr)
    {
        engine->start();
        currentTime = engine->getRealTime();
    }
}

void CoreApp::update()
{
    Date *jdate = engine->getDate();

    double  dt = jdate->update();
    vec3d_t av, tv;

    currentTime += dt;

    av = player->getAngularVelocity();
    tv = player->getTravelVelocity();

    // Mouse Wheel control
    // if (dollyMotion != 0.0)
    // {
    //     double span = 0.1;
    //     double fraction;

    //     if (currentTime - dollyTime >= span)
    //         fraction = (dollyTime + span) - (currentTime - dt);
    //     else
    //         fraction - dt / span;

    //     player->dolly(dollyMotion * fraction);
    //     if (currentTime - dollyTime >= span)
    //         dollyMotion = 0.0;
    // }

    // Keyboard attitude control
    // X-axis attitude control
    if (stateKey[keyPad2] || stateKey[keyDown])
        av += vec3d_t(dt * -keyAttitudeAccel, 0, 0);
    if (stateKey[keyPad8] || stateKey[keyUp])
        av += vec3d_t(dt * keyAttitudeAccel, 0, 0);
    
    // Y-axis attitude control
    if (stateKey[keyPad6] || stateKey[keyRight])
        av += vec3d_t(0, dt * -keyAttitudeAccel, 0);
    if (stateKey[keyPad4] || stateKey[keyLeft])
        av += vec3d_t(0, dt * keyAttitudeAccel, 0);

    // Z-axis attitude control
    if (stateKey[keyPad7])
        av += vec3d_t(0, 0, dt * -keyAttitudeAccel);
    if (stateKey[keyPad9])
        av += vec3d_t(0, 0, dt * keyAttitudeAccel);

    // Keyboard movement control
    // X-axis move control
    // if (stateKey[keyLeft])
    //     tv.x += dt * keyMovementControl;
    // if (stateKey[keyRight])
    //     tv.x -= dt * keyMovementControl;

    // Y-axis move control
    // if (stateKey[keyDown])
    //     tv.y += dt * keyMovementControl;
    // if (stateKey[keyUp])
    //     tv.y -= dt * keyMovementControl;

    // Z-axis move control
    if (stateKey[keyPad3] || stateKey[keyHome])
        tv.z += dt * keyMovementControl;
    if (stateKey[keyPad1] || stateKey[keyEnd])
        tv.z -= dt * keyMovementControl;

    // Braking control
    if (stateKey[keyPad5] || stateKey[keyb])
    {
        av *= exp(-dt * keyAttitudeBrake);
        tv *= exp(-dt * keyMovementBrake);
    }

    player->setAngularVelocity(av);
    player->setTravelVelocity(tv);

    // Keyboard orbit movement controls
    // {
    //     double coarseness = player->computeCoarseness(1.5);
    //     quatd_t q = { 1, 0, 0, 0 };

    //     if (shiftStateKey[keyLeft])
    //         q *= yRotate(dt * -keyRotationAccel * coarseness);
    //     if (shiftStateKey[keyRight])
    //         q *= yRotate(dt * keyRotationAccel * coarseness);
    //     if (shiftStateKey[keyUp])
    //         q *= xRotate(dt * -keyRotationAccel * coarseness);
    //     if (shiftStateKey[keyDown])
    //         q *= xRotate(dt * keyRotationAccel * coarseness);

    //     player->orbit(q);
    // }

    // Keyboard dolly control
    // if (shiftStateKey[keyHome])
    //     player->dolly(-dt * 2.0);
    // if (shiftStateKey[keyEnd])
    //     player->dolly(dt * 2.0);

    engine->update(dt);
}

void CoreApp::render()
{
    if (scene != nullptr)
        scene->render(*universe, *player);
}

// *****************
// Keyboard controls
// *****************

void CoreApp::keyPress(keyCode code, int modifiers, bool down)
{
    if (down == true)
    {
        if (modifiers & keyShift)
            shiftStateKey[code] = true;
        else if (modifiers & keyControl)
            ctrlStateKey[code] = true;
        else
            stateKey[code] = true;
    }
    else
    {
        stateKey[code] = false;
        shiftStateKey[code] = false;
        ctrlStateKey[code] = false;
    }
}

void CoreApp::keyEntered(char32_t ch, int modifiers)
{

}

