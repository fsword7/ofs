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
#include "main/keys.h"
#include "utils/json.h"

// Global variables
CoreApp *ofsAppCore = nullptr;
Logger *ofsLogger = nullptr;
TimeDate *ofsDate = nullptr;

CoreApp::CoreApp()
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

    return true;
}

void CoreApp::endTimeStep(bool running)
{
    // ofsLogger->info("End time updates\n");

    if (running)
        universe->finalizeUpdate();

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

void CoreApp::keyPress(char32_t code, int mods, bool down)
{
    static char32_t last = -1;

    if (code >= 512)
        return;

    // Logger::logger->debug("Key pressed {}: {}\n", int(code), down ? "Down" : "Up");

    if (down == true)
    {
        if (mods & ofs::keyButton::keyShift)
            shiftStateKey[code] = true;
        else if (mods & ofs::keyButton::keyControl)
            ctrlStateKey[code] = true;
        else if (mods & ofs::keyButton::keyAlt)
            altStateKey[code] = true;
        else
            stateKey[code] = true;

        // Check a key code while it is pressed down.
        if (last == code) // repeating - discard it.
            return;
        last = code;

        // Process a buffered key code once
        keyBufferedSystem(code, mods);
        if (bRunning)
            keyBufferedOnRunning(code, mods);
    }
    else
    {
        stateKey[code] = false;
        shiftStateKey[code] = false;
        ctrlStateKey[code] = false;
        altStateKey[code] = false;
        last = -1;
    }
}

void CoreApp::keyProcess(char32_t ch, int mods)
{
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
    if (stateKey[ofs::keyF5] || stateKey[ofs::key5])
        guimgr->showControl<DialogCamera>();

    if (player->isInternal()) {
        if (altStateKey[ofs::keyHome])
            player->resetCockpitDir();
        if (stateKey[ofs::keyF8] || stateKey[ofs::key8])
            panel->togglePanelMode();
        if (ctrlStateKey[ofs::keyH])
            panel->toggleHUD();
        if (stateKey[ofs::keyH])
            panel->switchHUDMode();
    }

    if (player->isExternal()) {
        if (player->getCameraMode() == camPersonalObserver) {
            if (stateKey[ofs::keyF8] || stateKey[ofs::key8])
                panel->togglePersonalPanelMode();
        } else {
             if (stateKey[ofs::keyF8] || stateKey[ofs::key8])
                panel->togglePlanetariumPanelMode();           
        }
        // Celestial *cbody = player->getReferenceObject();
        // if (altStateKey[ofs::keyN])
        //     cbody->toggleNormals();
    }
}

void CoreApp::keyBufferedOnRunning(char32_t key, int mods)
{

    // if (stateKey[keyCode::keyF3])
    //     increaseTimeWarp();
    // else if (stateKey[keyCode::keyF4])
    //     decreaseTimeWarp();

    if (stateKey[ofs::keyF4] || stateKey[ofs::key4])
        increaseTimeWarp();
    else if (stateKey[ofs::keyF3] || stateKey[ofs::key3])
        decreaseTimeWarp();
    else if (stateKey[ofs::keyF2] || stateKey[ofs::key2])
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
            if (stateKey[ofs::keyCode::keyPad8])
                av += glm::dvec3(dt * -keyAttitudeAccel, 0, 0);
            if (stateKey[ofs::keyCode::keyPad2])
                av += glm::dvec3(dt * keyAttitudeAccel, 0, 0);

            // Y-axis angular control
            if (stateKey[ofs::keyCode::keyPad4])
                av += glm::dvec3(0, dt * -keyAttitudeAccel, 0);
            if (stateKey[ofs::keyCode::keyPad6])
                av += glm::dvec3(0, dt * keyAttitudeAccel, 0);

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

        if (mode == camGroundObserver || mode == camPersonalObserver)
        {
            double dphi(0.0), dtheta(0.0);
        
            if (stateKey[ofs::keyCode::keyPad4])
                dphi += dt * -0.8;
            if (stateKey[ofs::keyCode::keyPad6])
                dphi += dt * 0.8;
            if (stateKey[ofs::keyCode::keyPad2])
                dtheta += dt * -0.8;
            if (stateKey[ofs::keyCode::keyPad8])
                dtheta += dt * 0.8;
            player->rotateView(dphi, dtheta);
        }

        if (mode == camPersonalObserver) {
            glm::dvec3 dm(0, 0, 0);
            double dh(0.0);

            if (stateKey[ofs::keyCode::keyLeft])
                dm.z -= dt * panSpeed;
            if (stateKey[ofs::keyCode::keyRight])
                dm.z += dt * panSpeed;
            if (stateKey[ofs::keyCode::keyUp])
                dm.x += dt * panSpeed;
            if (stateKey[ofs::keyCode::keyDown])
                dm.x -= dt * panSpeed;
            if (ctrlStateKey[ofs::keyCode::keyUp])
                dh += dt;
            if (ctrlStateKey[ofs::keyCode::keyDown])
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
        
        if (altStateKey[ofs::keyCode::keyPad4])
            dtheta += dt * 0.8;
        if (altStateKey[ofs::keyCode::keyPad6])
            dtheta += dt * -0.8;
        if (altStateKey[ofs::keyCode::keyPad2])
            dphi += dt * -0.8;
        if (altStateKey[ofs::keyCode::keyPad8])
            dphi += dt * 0.8;
        if (altStateKey[ofs::keyCode::keyPad7])
            dtilt += dt * -0.8;
        if (altStateKey[ofs::keyCode::keyPad9])
            dtilt += dt * 0.8;

        player->rotateCockpit(dphi, dtheta, dtilt);
    }
}

void CoreApp::keyImmediateOnRunning()
{
    double dt = td.getSysDeltaTime();

    // Clear all keyboard controls for thrusters
    // for (int idx = 0; idx < thgMaxThrusters; idx++)
    //     ctrlKeyThrusters[idx] = 0;

    // Reaction Control System controls
    // if (bEnableRCS)
    // {
        // rotation controls
        // if (stateKey[ofs::keyPad8])         ctrlKeyThrusters[thgRotPitchUp]      = 1000;
        // if (ctrlStateKey[ofs::keyPad8])     ctrlKeyThrusters[thgRotPitchUp]      = 100;
        // if (stateKey[ofs::keyPad2])         ctrlKeyThrusters[thgRotPitchDown]    = 1000;
        // if (ctrlStateKey[ofs::keyPad2])     ctrlKeyThrusters[thgRotPitchDown]    = 100;
        // if (stateKey[ofs::keyPad4])         ctrlKeyThrusters[thgRotYawLeft]      = 1000;
        // if (ctrlStateKey[ofs::keyPad4])     ctrlKeyThrusters[thgRotYawLeft]      = 100;
        // if (stateKey[ofs::keyPad6])         ctrlKeyThrusters[thgRotYawRight]     = 1000;
        // if (ctrlStateKey[ofs::keyPad6])     ctrlKeyThrusters[thgRotYawRight]     = 100;
        // if (stateKey[ofs::keyPad7])         ctrlKeyThrusters[thgRotBankLeft]     = 1000;
        // if (ctrlStateKey[ofs::keyPad7])     ctrlKeyThrusters[thgRotBankLeft]     = 100;
        // if (stateKey[ofs::keyPad9])         ctrlKeyThrusters[thgRotBankRight]    = 1000;
        // if (ctrlStateKey[ofs::keyPad9])     ctrlKeyThrusters[thgRotBankRight]    = 100;

        // linear controls
        // if (stateKey[ofs::keyUp])           ctrlKeyThrusters[thgLinMoveUp]       = 1000;
        // if (ctrlStateKey[ofs::keyUp])       ctrlKeyThrusters[thgLinMoveUp]       = 100;
        // if (stateKey[ofs::keyDown])         ctrlKeyThrusters[thgLinMoveDown]     = 1000;
        // if (ctrlStateKey[ofs::keyDown])     ctrlKeyThrusters[thgLinMoveDown]     = 100;
        // if (stateKey[ofs::keyLeft])         ctrlKeyThrusters[thgLinMoveLeft]     = 1000;
        // if (ctrlStateKey[ofs::keyLeft])     ctrlKeyThrusters[thgLinMoveLeft]     = 100;
        // if (stateKey[ofs::keyRight])        ctrlKeyThrusters[thgLinMoveRight]    = 1000;
        // if (ctrlStateKey[ofs::keyRight])    ctrlKeyThrusters[thgLinMoveRight]    = 100;
        // if (stateKey[ofs::keyPad1])         ctrlKeyThrusters[thgLinMoveForward]  = 1000;
        // if (ctrlStateKey[ofs::keyPad1])     ctrlKeyThrusters[thgLinMoveForward]  = 100;
        // if (stateKey[ofs::keyPad3])         ctrlKeyThrusters[thgLinMoveBackward] = 1000;
        // if (ctrlStateKey[ofs::keyPad3])     ctrlKeyThrusters[thgLinMoveBackward] = 100;

    // }

    // if (stateKey[ofs::keyF4])
    //     increaseTimeWarp();
    // else if (stateKey[ofs::keyF3])
    //     decreaseTimeWarp();
    // else if (stateKey[ofs::keyF2])
    //     setWarpFactor(1.0);

    Vehicle *veh = player->getVehicleTarget();

    // Main/Retro thruster controls
    if (ctrlStateKey[ofs::keyPadAdd])
        veh->throttleMainRetroThruster(0.2*dt);
    if (ctrlStateKey[ofs::keyPadSubtract])
        veh->throttleMainRetroThruster(-0.2*dt);
    if (altStateKey[ofs::keyPadAdd])
        veh->setMainRetroThruster(1.0);
    if (altStateKey[ofs::keyPadSubtract])
        veh->setMainRetroThruster(-1.0);
    if (stateKey[ofs::keyPadAdd])
        veh->overrideMainRetroThruster(1.0);
    if (stateKey[ofs::keyPadSubtract])
        veh->overrideMainRetroThruster(-1.0);
    if (stateKey[ofs::keyPadMultiply]) {
        veh->setThrustGroupLevel(thgMain, 0.0);
        veh->setThrustGroupLevel(thgRetro, 0.0);
    }

    // Hover thruster controls
    if (stateKey[ofs::keyPad0])
        veh->throttleThrustGroupLevel(thgHover, 0.2*dt);
    if (stateKey[ofs::keyPadDecimal])
        veh->throttleThrustGroupLevel(thgHover, -0.2*dt);

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
