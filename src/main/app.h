// app.h - Core application package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#include "main/timedate.h"
#include "engine/player.h"

class Engine;
class Universe;
class Player;
class Camera;
class View;
class Panel;
class GraphicsClient;
class GUIManager;
class Celestial;
class Vehicle;
class DialogCamera;


struct ModuleEntry
{
    std::string name;
    std::string path;
    ModuleHandle handle;
};

class CoreApp
{
public:
    CoreApp();
    virtual ~CoreApp() = default;
 
    // Virtual main function calls packages
    virtual void init();
    virtual void cleanup();
    virtual void run();

    inline GraphicsClient *getClient() { return gclient; }
    inline Camera    *getCamera() { return player->getCamera(); }
    inline Player    *getPlayer() { return player; }
    inline Universe  *getUniverse() { return universe; }

    void launch();
    void openSession(json &config);
    void closeSession();
    void updateWorld();
    void renderScene();
    // void render2D();
    void drawHUD();

    void setFocusingObject(Celestial *object);

    // Module function calls
    void loadStartupModules();
    ModuleHandle loadModule(cstr_t &path, cstr_t &name);
    bool unloadModule(ModuleHandle handle);
    bool unloadModule(cstr_t &name);
    void *findModuleProcAddress(ModuleHandle handle, cchar_t *funcName);

    bool attachGraphicsClient(GraphicsClient *gc);
    bool detachGraphicsClient(GraphicsClient *gc);
    void createSceneWindow();

    void displayFrame();
    void setWindowTitle(cstr_t &title);

    // Time/date update routines
    bool beginTimeStep(bool running);
    void endTimeStep(bool running);
    void setWarpFactor(double warp);
    void increaseTimeWarp();
    void decreaseTimeWarp();
    void pause(bool flag);
    void freeze(bool flag);
    void suspend();
    void resume();

    inline void togglePause() { pause(bRunning); }
    
    // Keyboard controls
    void keyPress(char32_t code, int modifiers, bool down);
    void keyProcess(char32_t ch, int modifiers);

    void processUserInputs();
    void keyBufferedSystem(char32_t key, int mods);
    void keyBufferedOnRunning(char32_t key, int mods);
    void keyImmediateSystem();
    void keyImmediateOnRunning();

    // Mouse contols
    void mouseMove(float mx, float my, int state);
    void mousePressButtonDown(float mx, float my, int state);
    void mousePressButtonUp(float mx, float my, int state);
    void mouseDialWheel(float motion, int state);

    // Joystick controls

    // Game controller controls

protected:
    Engine   *engine = nullptr;
    Universe *universe = nullptr;
    Player   *player = nullptr;
    // Camera   *camera = nullptr;
    // Camerax  *camerax = nullptr;

    GUIManager *guimgr = nullptr;
    DialogCamera *dlgCamera = nullptr;

    Panel    *panel = nullptr;

    GraphicsClient *gclient = nullptr;

    // Focusing vehicle object
    Celestial *focObject = nullptr;
    Vehicle *focVehicle = nullptr;
    Vehicle *pfocVehicle = nullptr;

    double currentTime = 0.0;
    std::chrono::time_point<std::chrono::system_clock> prevTime, suspendTime;

    bool bSession = false;
    bool bRunningApp = true;            // Running application
    bool bRunning = true;               // Pause/running 
    bool bRequestedRunning = true;      // Pending requested running toggle
    bool bFreezing = false;
    bool isPaused = false;

    TimeDate td;

    bool stateKey[512];
    bool shiftStateKey[512];
    bool ctrlStateKey[512];
    bool altStateKey[512];

    // int ctrlKeyThrusters[thgMaxThrusters];
    // int ctrlJoyThrusters[thgMaxThrusters];

    double keyAttitudeAccel = ofs::radians(5.0);
    double keyAttitudeBrake = ofs::radians(60.0);
    double keyRotationAccel = ofs::radians(60.0);
    // double keyMovementControl = 1000.0;
    double keyMovementControl = 25.0;     
    double keyMovementBrake = 10.0;

    double panSpeed = 0.05;

    // Mouse controls
    float xLast, yLast;

    // Wheel controls
    double dollyMotion = 0.0;
    double dollyTime = 0.0;
    double zoomMotion = 0.0;
    double zoomTime = 0.0;

    std::vector<View *> views;
    View *activeView = nullptr;

    std::vector<ModuleEntry> moduleList;
};

// extern CoreApp *ofsAppCore;