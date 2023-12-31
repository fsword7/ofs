// app.cpp - Core application main routines
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/graphics.h"
#include "api/logger.h"
#include "engine/engine.h"
#include "engine/player.h"
#include "engine/view.h"
#include "universe/universe.h"
// #include "render/scene.h"
#include "control/panel.h"
#include "control/hudpanel.h"
#include "main/guimgr.h"
#include "main/app.h"
#include "main/keys.h"

// Global variables
CoreApp *ofsAppCore = nullptr;

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
    // player = new Player();
    // engine->init(ctx, width, height);

    // scene = engine->getScene();
    universe = engine->getUniverse();
    // player = engine->getPlayer();
    // camera = player->getCamera();

 
    View *view = new View(View::viewMainWindow, 0.0f, 0.0f, 1.0f, 1.0f);
    views.push_back(view);
    activeView = view;
}

View *CoreApp::pickView(float x, float y)
{
    if (views.size() > 0)
        return views[0];
    return nullptr;
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
    // Date *jdate = engine->getDate();

    // double  dt = jdate->update();
    // vec3d_t av, tv;

    // currentTime += dt;

    // av = player->getAngularVelocity();
    // tv = player->getTravelVelocity();

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
    // if (stateKey[keyPad2] || stateKey[keyDown])
    //     av += vec3d_t(dt * keyAttitudeAccel, 0, 0);
    // if (stateKey[keyPad8] || stateKey[keyUp])
    //     av += vec3d_t(dt * -keyAttitudeAccel, 0, 0);
    
    // // Y-axis attitude control
    // if (stateKey[keyPad6] || stateKey[keyRight])
    //     av += vec3d_t(0, dt * keyAttitudeAccel, 0);
    // if (stateKey[keyPad4] || stateKey[keyLeft])
    //     av += vec3d_t(0, dt * -keyAttitudeAccel, 0);

    // // Z-axis attitude control
    // if (stateKey[keyPad7])
    //     av += vec3d_t(0, 0, dt * -keyAttitudeAccel);
    // if (stateKey[keyPad9])
    //     av += vec3d_t(0, 0, dt * keyAttitudeAccel);

    // // Keyboard movement control
    // // X-axis move control
    // if (shiftStateKey[keyPad4])
    //     tv.x() += dt * keyMovementControl;
    // if (shiftStateKey[keyPad6])
    //     tv.x() -= dt * keyMovementControl;

    // // Y-axis move control
    // if (shiftStateKey[keyPad8])
    //     tv.y() += dt * keyMovementControl;
    // if (shiftStateKey[keyPad2])
    //     tv.y() -= dt * keyMovementControl;

    // Z-axis move control
    // if (stateKey[keyPad3])
    //     tv.z() += dt * keyMovementControl;
    // if (stateKey[keyPad1])
    //     tv.z() -= dt * keyMovementControl;

    // // Braking control
    // if (stateKey[keyPad5] || stateKey[keyb])
    // {
    //     av *= exp(-dt * keyAttitudeBrake);
    //     tv *= exp(-dt * keyMovementBrake);
    // }

    // player->setAngularVelocity(av);
    // player->setTravelVelocity(tv);

    // // Keyboard orbit movement controls
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

    //     if (q != quatd_t(1, 0, 0, 0))
    //         player->orbit(q);
    // }

    // // Keyboard dolly control
    // if (shiftStateKey[keyHome])
    //     player->dolly(-dt * 2.0);
    // if (shiftStateKey[keyEnd])
    //     player->dolly(dt * 2.0);

    // engine->update(dt);
}

void CoreApp::render()
{
    // if (scene != nullptr)
    //     scene->render(*universe, *player);
    // engine->renderOverlay();
}

// ********* new package *********

// Logger *Logger::logger = nullptr;
// Logger *logger = nullptr;

Logger *ofsLogger = nullptr;

void CoreApp::init()
{
    ofsLogger = new Logger(Logger::logDebug, "ofs.log");

    if (glfwInit() != GLFW_TRUE)
    {
        ofsLogger->fatal("OFS: Unable to initialize GLFW interface.\n");
        abort();
    }
    ofsLogger->info("OFS: Loaded GLFW version: {}\n",
        glfwGetVersionString());

    // camera = new Camera(width, height);

    // Loading startup modules
    loadStartupModules();

    // Loading plugin modules
    loadModule("modules/plugin", "glclient");

    // Initialize graphics client module
    if (gclient != nullptr)
        createSceneWindow();

    // Initialize universe
    universe = new Universe();
    universe->init();
}

void CoreApp::cleanup()
{
    // Unloading modules

    if (guimgr != nullptr)
        delete guimgr;
}

void CoreApp::openSession()
{
    // Starting current time from system time
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> nowTime = now.time_since_epoch();
    // Logger::getLogger()->info("Today MJD Time: {} => {}\n",
    //     astro::MJD(nowTime.count()), astro::getMJDDateStr(astro::MJD(nowTime.count())));
    td.reset(nowTime.count());
    prevTime = now;

    player = new Player(&td);

    if (gclient != nullptr)
    {
        gclient->cbStart(universe);
        gclient->showWindow();
    }

    bSession = true;
    bStarted = true;
}

void CoreApp::closeSession()
{
    if (gclient != nullptr)
        gclient->hideWindow();

    if (player != nullptr)
        delete player;
    player = nullptr;

    bSession = false;
}

void CoreApp::updateWorld()
{
    universe->update(player, td);
    if (bStarted)
    {
        universe->start(td);
        bStarted = false;
    }
    player->update(td);
}

bool CoreApp::attachGraphicsClient(GraphicsClient *gc)
{
    if (gclient != nullptr)
        return false;
    gclient = gc;
    gclient->cbInitialize();
    return true;
}

bool CoreApp::detachGraphicsClient(GraphicsClient *gc)
{
    if (gclient != gc || gclient == nullptr)
        return false;
    gclient = nullptr;
    return true;
}

void CoreApp::createSceneWindow()
{
    if (gclient == nullptr)
        exit(EXIT_FAILURE);
    guimgr = new GUIManager(gclient);

    // Initialize callbacks for window events
    guimgr->setupCallbacks();
}

void CoreApp::renderScene()
{
    if (gclient != nullptr)
        gclient->cbRenderScene(player);
}

void CoreApp::drawHUD()
{
    if (bSession && panel != nullptr)
        panel->drawHUD();
}

void CoreApp::displayFrame()
{
    if (gclient != nullptr)
        gclient->cbDisplayFrame();
}

void CoreApp::setWindowTitle(cstr_t &title)
{
    if (gclient != nullptr)
        gclient->cbSetWindowTitle(title);
}

void CoreApp::run()
{
    openSession();

    while (!guimgr->shouldClose())
    {
        // Process polling events
        guimgr->pollEvents();
        
        if (bSession)
        {
            if (beginTimeStep(bRunning))
            {
                updateWorld();
                endTimeStep(bRunning);
            }

            processUserInputs();
        }

        renderScene();
        drawHUD();
        guimgr->render();
        displayFrame();
    }

    closeSession();
}

// ******** Time/date updating routines/controls ********

bool CoreApp::beginTimeStep(bool running)
{
    // Check for a pause/resume request
    if (bRequestedRunning != running)
    {
        running = bRunning = bRequestedRunning;
        isPaused = !running;
    }

    double dt = 0.0;
    auto now = std::chrono::system_clock::now();

    std::chrono::duration<double> timeDelta = now - prevTime;
    dt = timeDelta.count();

    prevTime = now;
    td.beginStep(dt, running);

    return true;
}

void CoreApp::endTimeStep(bool running)
{
    if (running)
        universe->finalizeUpdate();

    td.endStep(running);
}

void CoreApp::pause(bool bPause)
{
    if (bRunning != bPause)
        return;
    bRequestedRunning = !bPause;
}

void CoreApp::freeze(bool bFreeze)
{
    if (bRunning != bFreeze)
        return;
    bRunning = !bFreeze;

    if (bFreeze)
        suspend();
    else
        resume();
}

void CoreApp::suspend()
{
    suspendTime = std::chrono::system_clock::now();
}

void CoreApp::resume()
{
    auto deltaTime = std::chrono::system_clock::now() - suspendTime;
    prevTime += deltaTime;
}

// ******** Module function calls ********

void CoreApp::loadStartupModules()
{
    fs::path path = "modules/startup";

}

ModuleHandle CoreApp::loadModule(cstr_t &path, cstr_t &name)
{
#ifdef __WIN32__
    std::string fname = fmt::format("{}/lib{}.dll", path, name);
#else /* __WIN32__ */
    std::string fname = fmt::format("{}/lib{}.so", path, name);
#endif /* __WIN32__ */
    ModuleHandle handle = ofsLoadModule(fname.c_str());

    if (handle == nullptr)
    {
        printf("Failed loading module %s: %s\n",
            name.c_str(), ofsGetModuleError());
        return nullptr;
    }

    // Start initialization routine in module.
    // void (*initModule)(ModuleHandle) =
    //     (void(*)(ModuleHandle))ofsGetProcAddress(handle, "initModule");
    // printf("initModule - %p (%p)\n", initModule, handle);
    // if (initModule != nullptr)
    //     initModule(handle);

    // Add module entry to list
    ModuleEntry entry;
    entry.name   = name;
    entry.path   = fname;
    entry.handle = handle;

    moduleList.push_back(entry);

    return handle;
}

bool CoreApp::unloadModule(ModuleHandle handle)
{
    for (auto it = moduleList.begin(); it != moduleList.end(); it++)
    {
        if (it->handle != handle)
            continue;

        // Found, start cleanup routine in module.
        // void (*opcCleanup)(ModuleHandle) =
        //     (void(*)(ModuleHandle))ofsGetProcAddress(it->handle, "opcCleanup");
        // if (opcCleanup != nullptr)
        //     opcCleanup(it->handle);

        // Remove it from module list and unload module.
        moduleList.erase(it);
        ofsUnloadModule(it->handle);

        return true;
    }

    return false;
}

bool CoreApp::unloadModule(cstr_t &name)
{
    for (auto it = moduleList.begin(); it != moduleList.end(); it++)
    {
        if (it->name != name)
            continue;

        // Found, start cleanup routine in module.
        void (*opcCleanup)(ModuleHandle) =
            (void(*)(ModuleHandle))ofsGetProcAddress(it->handle, "opcCleanup");
        if (opcCleanup != nullptr)
            opcCleanup(it->handle);

        // Remove it from module list and unload module.
        moduleList.erase(it);
        ofsUnloadModule(it->handle);

        return true;
    }

    return false;
}

void *CoreApp::findModuleProcAddress(ModuleHandle handle, cchar_t *funcName)
{
    return ofsGetProcAddress(handle, funcName);
}

// ******** Keyboard Controls ********

void CoreApp::keyPress(char32_t code, int modifiers, bool down)
{
    if (code >= 512)
        return;

    // Logger::logger->debug("Key pressed {}: {}\n", int(code), down ? "Down" : "Up");

    if (down == true)
    {
        if (modifiers & ofs::keyButton::keyShift)
            shiftStateKey[code] = true;
        else if (modifiers & ofs::keyButton::keyControl)
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

void CoreApp::keyProcess(char32_t ch, int modifiers)
{
    // switch (ch)
    // {
    //     case L'j':  // Reset time warp scale
    //         engine->setTimeWarp(1);
    //         break;

    //     case L'k':  // Increase time warp scale
    //         engine->setTimeWarp(engine->getTimeWarp() * 2.0);
    //         break;

    //     case L'l':  // Decrease time warp scale
    //         engine->setTimeWarp(engine->getTimeWarp() / 2.0);
    //         break;
    // }
}

void CoreApp::processUserInputs()
{
    // process keyboard controls
    keyImmediateSystem();
    if (bRunning)
        keyImmediateOnRunning();
}

void CoreApp::keyBufferedSystem(char32_t key, int mods)
{

}

void CoreApp::keyBufferedOnRunning(char32_t key, int mods)
{

}

void CoreApp::keyImmediateSystem()
{
    double dt = td.getSysDeltaTime();

    if (player->isExternal())
    {
        // External camera view

        glm::dvec3 av = player->getAngularControl();
        glm::dvec3 tv = player->getTravelControl();

        // Keyboard angular conrtrol
        // X-axis angular control
        if (stateKey[ofs::keyCode::keyPad8])
            av += glm::dvec3(dt * keyAttitudeAccel, 0, 0);
        if (stateKey[ofs::keyCode::keyPad2])
            av += glm::dvec3(dt * -keyAttitudeAccel, 0, 0);

        // Y-axis angular control
        if (stateKey[ofs::keyCode::keyPad4])
            av += glm::dvec3(0, dt * keyAttitudeAccel, 0);
        if (stateKey[ofs::keyCode::keyPad6])
            av += glm::dvec3(0, dt * -keyAttitudeAccel, 0);

        // Z-axis angular control
        if (stateKey[ofs::keyCode::keyPad7])
            av += glm::dvec3(0, 0, dt * -keyAttitudeAccel);
        if (stateKey[ofs::keyCode::keyPad9])
            av += glm::dvec3(0, 0, dt * keyAttitudeAccel);

        // Keyboard movement control
        // // X-axis move control
        // if (shiftStateKey[keyPad4])
        //     tv.x() += dt * keyMovementControl;
        // if (shiftStateKey[keyPad6])
        //     tv.x() -= dt * keyMovementControl;

        // // Y-axis move control
        // if (shiftStateKey[keyPad8])
        //     tv.y() += dt * keyMovementControl;
        // if (shiftStateKey[keyPad2])
        //     tv.y() -= dt * keyMovementControl;

        // Z-axis move control
        if (stateKey[ofs::keyCode::keyPad3])
            tv.z += dt * keyMovementControl;
        if (stateKey[ofs::keyCode::keyPad1])
            tv.z -= dt * keyMovementControl;

        // Braking control
        if (stateKey[ofs::keyCode::keyPad5] || stateKey[ofs::keyCode::keyb])
        {
            av *= exp(-dt * keyAttitudeBrake);
            tv *= exp(-dt * keyMovementBrake);
        }

        player->setAngularControl(av);
        player->setTravelControl(tv);

        // Keyboard orbit movement controls
        {
            double coarseness = player->computeCoarseness(1.5);
            glm::dquat q = { 1, 0, 0, 0 };

            if (stateKey[ofs::keyCode::keyLeft])
                q *= yqRotate(dt * -keyRotationAccel * coarseness);
            if (stateKey[ofs::keyCode::keyRight])
                q *= yqRotate(dt * keyRotationAccel * coarseness);
            if (stateKey[ofs::keyCode::keyUp])
                q *= xqRotate(dt * -keyRotationAccel * coarseness);
            if (stateKey[ofs::keyCode::keyDown])
                q *= xqRotate(dt * keyRotationAccel * coarseness);

            if (q != glm::dquat(1, 0, 0, 0))
                player->orbit(q);
        }

        // Keyboard dolly control
        if (stateKey[ofs::keyCode::keyHome])
            player->dolly(-dt * 2.0);
        if (stateKey[ofs::keyCode::keyEnd])
            player->dolly(dt * 2.0);
    }
    else
    {
        // Internal camera view (in cocpkit)
    }
}

void CoreApp::keyImmediateOnRunning()
{

}

// ******** Mouse Controls ********

void CoreApp::mouseMove(float mx, float my, int state)
{

}

void CoreApp::mousePressButtonDown(float mx, float my, int state)
{

}

void CoreApp::mousePressButtonUp(float mx, float my, int state)
{
    // View *view = nullptr;
    // float vx = 0.0f, vy = 0.0f;

    // if (state & mouseLeftButton)
    // {
    //     view = pickView(mx, my);
    //     if (view != nullptr)
    //         view->map(mx / float(width), my / float(height), vx, vy);

    //     vec3d_t ray = player->getPickRay(vx, vy);

    //     Object *picked = engine->pickObject(ray);
    // }
}

void CoreApp::mouseDialWheel(float motion, int state)
{

}

// ******** Joystick Controls ********

// ******** Gamepad Controls ********
