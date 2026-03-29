// app.cpp - Core application main routines
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/graphics.h"
#include "api/logger.h"
#include "engine/engine.h"
#include "engine/vehicle/vehicle.h"
#include "engine/player.h"
#include "engine/view.h"
#include "universe/universe.h"
#include "universe/astro.h"
// #include "render/scene.h"
#include "control/panel.h"
#include "engine/dlgcam.h"
#include "main/guimgr.h"
#include "main/app.h"
#include "utils/json.h"

// Global variables
CoreApp *ofsAppCore = nullptr;
Logger *ofsLogger = nullptr;
TimeDate *ofsDate = nullptr;
bool ofsStateUpdate = false;

CoreApp::CoreApp()
{
    // Initialize state keys
    for (int idx = 0; idx < ARRAY_SIZE(keyState); idx++)
        keyState[idx] = false;
}

void CoreApp::init()
{
    ofsLogger = new Logger(Logger::logDebug, "ofs.log");
    ofsDate = &td;

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
#ifdef OFS_LIBRARY_DIR
    str_t libpath = OFS_LIBRARY_DIR;
    loadModule(libpath + "/plugin", "glclient");
#else
    loadModule("modules/plugin", "glclient");
#endif

    // Initialize graphics client module
    if (gclient != nullptr)
        createSceneWindow();
}

void CoreApp::cleanup()
{
    // Unloading modules

    if (guimgr != nullptr)
        delete guimgr;
}

void CoreApp::setFocusingObject(Celestial *object)
{
    focObject = object;
    if (object->getType() == objVehicle)
    {
        pfocVehicle = focVehicle;
        focVehicle = dynamic_cast<Vehicle *>(object);
    }
}

void CoreApp::launch()
{
    fs::path homePath = OFS_HOME_DIR;
    fs::path startPath = homePath / "scen/start.json";
    ofsLogger->info("Open file: {}\n", startPath.string());
    std::ifstream inFile(homePath / "scen/start.json");
    if (!inFile.is_open()) {
        ofsLogger->info("File {}: {} - aborted\n",
            startPath.string(), strerror(errno));
        abort();
    }
    json config = json::parse(inFile, nullptr, false, true);

    universe = new Universe();
    universe->init();
    universe->configure(config);
    openSession(config);
}

void CoreApp::openSession(json &config)
{
    ofsLogger->info("Now running the world\n");

    // Starting date/time from system time or json config file 
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> nowTime = now.time_since_epoch();
    double mjdref = astro::MJD(nowTime.count());
    mjdref = myjson::getFloat<double>(config, "mjd", mjdref);
    ofsLogger->info("Starting MJD Time: {} Date: {}\n",
        mjdref, astro::getMJDDateStr(mjdref));
    td.reset(nowTime.count(), mjdref);
    prevTime = now;

    VideoData *video = gclient->getVideoData();

    player = new Player(&td, video->width, video->height);
    panel = new Panel(gclient, video->width, video->height, 8);
    guimgr->setPlayer(player);

    // initializing solar system with
    // new time for that session.
    universe->start();

    // Now load and configure vehicles (ships)
    universe->configureVehicles(config);

    // Now configure player
    json pconfig;
    if (config.contains("player"))
        pconfig = config["player"];
    else {
        // Get default player configurations
        ofsLogger->info("Use default player configuration\n");
        fs::path homePath = OFS_HOME_DIR;
        std::ifstream inFile(homePath / "scen/player.json");
        pconfig = json::parse(inFile, nullptr, false, true);
    }
    player->configure(pconfig);

    if (gclient != nullptr) {
        gclient->cbStart(universe);
        gclient->showWindow();
    }

    panel->init(config);
    if (player->isExternal())
        panel->setPanelMode(PANEL_PLANET);

    // Finalize all vehicles after creation
    universe->finalizePostCreation();

    bSession = true;

    // beginTimeStep(true);
    // updateWorld();
    // endTimeStep(true);

    ofsLogger->info("\nStarting of sesssion\n");
    ofsLogger->info("--------------------\n");
}

void CoreApp::closeSession()
{
    ofsLogger->info("-----------------\n");
    ofsLogger->info("Ending of session\n\n");

    if (gclient != nullptr)
        gclient->hideWindow();

    if (panel != nullptr)
        delete panel;
    if (player != nullptr)
        delete player;
    player = nullptr;
    panel = nullptr;

    bSession = false;
}

void CoreApp::updateWorld()
{
    universe->update(player, td);
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

    dlgCamera = new DialogCamera("Camera");

    guimgr->registerControl(dlgCamera);

}

void CoreApp::renderScene()
{
    if (gclient != nullptr)
        gclient->cbRenderScene(player);
    // Rendering 2D overlay panel
    if (panel != nullptr)
        panel->render(*player);
}

// void CoreApp::render2D()
// {
//     if (panel != nullptr)
//         panel->render(player);
// }

void CoreApp::drawHUD()
{
    if (bSession && panel != nullptr)
        panel->drawHUD(*player);
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
    // ofsLogger->info("Begin time updates\n");

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

    // ofsLogger->info("Current MJD Time: {:f} Date: {}\n", td.getMJD1(),
    //     astro::getMJDDateStr(td.getMJD1()));

    ofsStateUpdate = true;
    return true;
}

void CoreApp::endTimeStep(bool running)
{
    // ofsLogger->info("End time updates\n");

    if (running)
        universe->finalizeUpdate();
    ofsStateUpdate = false;

    td.endStep(running);

    if (panel != nullptr)
        panel->update(*player, td.getSimTime1(), td.getSysTime1());
}

void CoreApp::setWarpFactor(double warp)
{
    if (warp == td.getTimeWarp())
        return;
    const double eps = 1e-6;
    if (fabs(warp - td.getTimeWarp()) > eps)
        td.setTimeWarp(warp);
}

void CoreApp::increaseTimeWarp()
{
    const double eps = 1e-6;
    double logtw = log10(td.getTimeWarp());
    setWarpFactor(pow(10.0, floor(logtw+eps)+1.0));
}

void CoreApp::decreaseTimeWarp()
{
    const double eps = 1e-6;
    double logtw = log10(td.getTimeWarp());
    setWarpFactor(pow(10.0, floor(logtw+eps)-1.0));
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
#ifdef OFS_LIBRARY_DIR
    fs::path libpath = OFS_LIBRARY_DIR;
    libpath += "/startup";
#else
    fs::path libpath = "modules/startup";
#endif

}

ModuleHandle CoreApp::loadModule(cstr_t &path, cstr_t &name)
{
#ifdef __WIN32__
    std::string fname = std::format("{}/lib{}.dll", path, name);
#else /* __WIN32__ */
    std::string fname = std::format("{}/lib{}.so", path, name);
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

void CoreApp::keyPress(uint8_t key, bool down)
{
    // Logger::logger->debug("Key pressed {}: {}\n", int(code), down ? "Down" : "Up");

    if (!bSession)
        return;

    keyState[key] = down;

    if (down == true) {
        // Process a buffered key code once
        keyBufferedSystem(key);
        if (bRunning)
            keyBufferedOnRunning(key);
    }
}

void CoreApp::processUserInputs()
{
    // process keyboard controls
    keyImmediateSystem();
    if (bRunning)
        keyImmediateOnRunning();

    if (bSession) {
        Vehicle *veh = player->getVehicleTarget();
        veh->updateUserAttitudeControls();
    }
}

void CoreApp::keyBufferedSystem(uint8_t key)
{
    // if (stateKey[ofs::keyF5] || stateKey[ofs::key5])
    //     guimgr->showControl<DialogCamera>();

    if (player->isInternal()) {
        if (keymap.isLogicalKey(key, keyState, ofs::lkeyObserverResetHome))
            player->resetCockpitDir();
        if (keymap.isLogicalKey(key, keyState, ofs::lkeyTogglePanelMode))
            panel->togglePanelMode();
        if (keymap.isLogicalKey(key, keyState, ofs::lkeyToggleHUDMode))
            panel->toggleHUD();
        if (keymap.isLogicalKey(key, keyState, ofs::lkeySwitchHUDMode))
            panel->switchHUDMode();
    }

    if (player->isExternal()) {
        if (player->getCameraMode() == camPersonalObserver) {
            if (keymap.isLogicalKey(key, keyState, ofs::lkeyTogglePanelMode))
                panel->togglePersonalPanelMode();
        } else {
             if (keymap.isLogicalKey(key, keyState, ofs::lkeyTogglePanelMode))
                panel->togglePlanetariumPanelMode();           
        }
        // Celestial *cbody = player->getReferenceObject();
        // if (altStateKey[ofs::keyN])
        //     cbody->toggleNormals();
    }
}

void CoreApp::keyBufferedOnRunning(uint8_t key)
{
    Vehicle *veh = player->getVehicleTarget();

    veh->processBufferedKeyOnRunning(key, keyState, keymap);

    if (keymap.isLogicalKey(key, keyState, ofs::lkeyIncWarpTime))
        increaseTimeWarp();
    else if (keymap.isLogicalKey(key, keyState, ofs::lkeyDecWarpTime))
        decreaseTimeWarp();
    else if (keymap.isLogicalKey(key, keyState, ofs::lkeyResetWarpTime))
        setWarpFactor(1.0);

}

void CoreApp::keyImmediateSystem()
{
    double dt = td.getSysDeltaTime();

    if (player->isExternal())
    {
        // External camera view
        cameraMode mode = player->getCameraMode();
        if (mode == camGlobalFrame || mode == camTargetRelative || 
            mode == camTargetUnlocked || mode == camTargetSync)
        {
            glm::dvec3 av = player->getAngularControl();
            glm::dvec3 tv = player->getTravelControl();

            // Keyboard angular conrtrol
            // X-axis angular control
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelTurnUp))
                av += glm::dvec3(dt * -keyAttitudeAccel, 0, 0);
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelTurnDown))
                av += glm::dvec3(dt * keyAttitudeAccel, 0, 0);

            // Y-axis angular control
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelTurnLeft))
                av += glm::dvec3(0, dt * -keyAttitudeAccel, 0);
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelTurnRight))
                av += glm::dvec3(0, dt * keyAttitudeAccel, 0);

            // Z-axis angular control
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelTiltLeft))
                av += glm::dvec3(0, 0, dt * -keyAttitudeAccel);
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelTiltRight))
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
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelMoveForward))
                tv.z += dt * keyMovementControl;
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelMoveBackward))
                tv.z -= dt * keyMovementControl;

            // Braking control
            if (keymap.isLogicalKey(keyState, ofs::lkeyTravelBrake))
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

                if (keymap.isLogicalKey(keyState, ofs::lkeyOrbitMoveLeft))
                    q *= yqRotate(dt * -keyRotationAccel * coarseness);
                if (keymap.isLogicalKey(keyState, ofs::lkeyOrbitMoveRight))
                    q *= yqRotate(dt * keyRotationAccel * coarseness);
                if (keymap.isLogicalKey(keyState, ofs::lkeyOrbitMoveUp))
                    q *= xqRotate(dt * -keyRotationAccel * coarseness);
                if (keymap.isLogicalKey(keyState, ofs::lkeyOrbitMoveDown))
                    q *= xqRotate(dt * keyRotationAccel * coarseness);

                if (q != glm::dquat(1, 0, 0, 0))
                    player->orbit(q);
            }

            // Keyboard dolly control
            if (keymap.isLogicalKey(keyState, ofs::lkeyDollyMoveForward))
                player->dolly(-dt * 2.0);
            if (keymap.isLogicalKey(keyState, ofs::lkeyDollyMoveBackward))
                player->dolly(dt * 2.0);
        }

        if (mode == camGroundObserver || mode == camPersonalObserver)
        {
            double dphi(0.0), dtheta(0.0);
        
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnLeft))
                dphi += dt * -0.8;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnRight))
                dphi += dt * 0.8;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnDown))
                dtheta += dt * -0.8;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnUp))
                dtheta += dt * 0.8;
            player->rotateView(dphi, dtheta);
        }

        if (mode == camPersonalObserver) {
            glm::dvec3 dm(0, 0, 0);
            double dh(0.0);

            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnLeft))
                dm.z -= dt * panSpeed;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnRight))
                dm.z += dt * panSpeed;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnUp))
                dm.x += dt * panSpeed;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnDown))
                dm.x -= dt * panSpeed;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverMoveUp))
                dh += dt;
            if (keymap.isLogicalKey(keyState, ofs::lkeyObserverMoveDown))
                dh -= dt;

            if (glm::length(dm) != 0)
                player->shiftPersonalObserver(dm, dh);
        }

            // glm::dvec3 av = player->getGroundAngularControl();
            // glm::dvec3 tv = player->getGroundTravelControl();

            // // Keyboard angular conrtrol
            // // X-axis angular control
            // if (stateKey[ofs::keyCode::keyPad8])
            //     av += glm::dvec3(dt * -keyAttitudeAccel, 0, 0);
            // if (stateKey[ofs::keyCode::keyPad2])
            //     av += glm::dvec3(dt * keyAttitudeAccel, 0, 0);

            // // Y-axis angular control
            // if (stateKey[ofs::keyCode::keyPad4])
            //     av += glm::dvec3(0, dt * -keyAttitudeAccel, 0);
            // if (stateKey[ofs::keyCode::keyPad6])
            //     av += glm::dvec3(0, dt * keyAttitudeAccel, 0);

            // // Z-axis angular control
            // if (stateKey[ofs::keyCode::keyPad7])
            //     av += glm::dvec3(0, 0, dt * -keyAttitudeAccel);
            // if (stateKey[ofs::keyCode::keyPad9])
            //     av += glm::dvec3(0, 0, dt * keyAttitudeAccel);
 
            // // Braking control
            // if (stateKey[ofs::keyCode::keyPad5] || stateKey[ofs::keyCode::keyb])
            // {
            //     av *= exp(-dt * keyAttitudeBrake);
            //     // tv *= exp(-dt * keyMovementBrake);
            // }

            // player->setGroundAngularControl(av);
            // player->setGroundTravelControl(tv);
    }
    else
    {
        // Internal camera view (in cocpkit)
        double dphi(0.0), dtheta(0.0), dtilt(0.0);

        if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnLeft))
            dtheta += dt * 0.8;
        if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnRight))
            dtheta += dt * -0.8;
        if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnUp))
            dphi += dt * 0.8;
        if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTurnDown))
            dphi += dt * -0.8;
        if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTiltLeft))
            dtilt += dt * -0.8;
        if (keymap.isLogicalKey(keyState, ofs::lkeyObserverTiltRight))
            dtilt += dt * 0.8;

        player->rotateCockpit(dphi, dtheta, dtilt);
    }
}

void CoreApp::keyImmediateOnRunning()
{
    double dt = td.getSysDeltaTime();

    Vehicle *veh = player->getVehicleTarget();

    veh->processImmediateKeyOnRunning(keyState, keymap);

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
