// app.h - Core application package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#include "main/timedate.h"
#include <SDL2/SDL.h>

class Context;
class Engine;
class Universe;
class Player;
class Camera;
class Scene;
class View;
class Panel;
class GraphicsClient;

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
 
    enum mouseButton
    {
        mouseLeftButton     = 0x01,
        mouseMiddleButton   = 0x02,
        mouseRightButton    = 0x04,
        mouseShiftButton    = 0x08,
        mouseControlButton  = 0x10,
        mouseAltButton      = 0x20
    };

    enum keyButton
    {
        keyShift   = 0x01,
        keyControl = 0x02,
        keyAlt     = 0x04
    };

    enum keyCode
    {
        keyUndefined    = 0x000,

        // 0x00-0xFF reserved for ASCII codes

        kay0            = 0x030,
        key1            = 0x031,
        key2            = 0x032,
        key3            = 0x033,
        key4            = 0x034,
        key5            = 0x035,
        key6            = 0x036,
        key7            = 0x037,
        key8            = 0x038,
        key9            = 0x039,

        kayA            = 0x041,
        keyB            = 0x042,
        keyC            = 0x043,
        keyD            = 0x044,
        keyE            = 0x045,
        keyF            = 0x046,
        keyG            = 0x047,
        keyH            = 0x048,
        keyI            = 0x049,
        keyJ            = 0x04A,
        keyK            = 0x04B,
        keyL            = 0x04C,
        keyM            = 0x04D,
        keyN            = 0x04E,
        keyO            = 0x04F,
        keyP            = 0x050,
        keyQ            = 0x051,
        keyR            = 0x052,
        keyS            = 0x053,
        keyT            = 0x054,
        keyU            = 0x055,
        keyV            = 0x056,
        keyW            = 0x057,
        keyX            = 0x058,
        keyY            = 0x059,
        keyZ            = 0x05A,

        kaya            = 0x061,
        keyb            = 0x062,
        keyc            = 0x063,
        keyd            = 0x064,
        keye            = 0x065,
        keyf            = 0x066,
        keyg            = 0x067,
        keyh            = 0x068,
        keyi            = 0x069,
        keyj            = 0x06A,
        keyk            = 0x06B,
        keyl            = 0x06C,
        keym            = 0x06D,
        keyn            = 0x06E,
        keyo            = 0x06F,
        keyp            = 0x070,
        keyq            = 0x071,
        keyr            = 0x072,
        keys            = 0x073,
        keyt            = 0x074,
        keyu            = 0x075,
        keyv            = 0x076,
        keyw            = 0x077,
        keyx            = 0x078,
        keyy            = 0x079,
        keyz            = 0x07A,

        keyF1           = 0x101,
        keyF2           = 0x102,
        keyF3           = 0x103,
        keyF4           = 0x104,
        keyF5           = 0x105,
        keyF6           = 0x106,
        keyF7           = 0x107,
        keyF8           = 0x108,
        keyF9           = 0x109,
        keyF10          = 0x10A,
        keyF11          = 0x10B,
        keyF12          = 0x10C,

        keyPad0         = 0x110,
        keyPad1         = 0x111,
        keyPad2         = 0x112,
        keyPad3         = 0x113,
        keyPad4         = 0x114,
        keyPad5         = 0x115,
        keyPad6         = 0x116,
        keyPad7         = 0x117,
        keyPad8         = 0x118,
        keyPad9         = 0x119,

        keyUp           = 0x120,
        keyDown         = 0x121,
        keyLeft         = 0x122,
        keyRight        = 0x123,
        keyHome         = 0x124,
        keyEnd          = 0x125,
        keyPageUp       = 0x126,
        keyPageDown     = 0x127,
        keyInsert       = 0x128,
        keyDelete       = 0x129
    };

    // Virtual main function calls packages
    virtual void init();
    virtual void cleanup();
    virtual void run();

    inline GraphicsClient *getClient() { return gclient; }
    inline Universe *getUniverse() { return universe; }
    
    void initEngine();

    View *pickView(float x, float y);

    void start();
    void update();
    void render();

    void openSession();
    void closeSession();
    void updateWorld();
    void renderScene();
    void drawHUD();

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

    // Time/date update routines
    bool beginTimeStep(bool running);
    void endTimeStep(bool running);
    void pause(bool flag);
    void freeze(bool flag);
    void suspend();
    void resume();

    inline void togglePause() { pause(bRunning); }

    // Keyboard controls
    void keyPress(keyCode code, int modifiers, bool down);
    void keyEntered(char32_t ch, int modifiers);

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
    Camera   *camera = nullptr;

    Context  *ctx = nullptr;
    Scene    *scene = nullptr;
    Panel    *panel = nullptr;

    GraphicsClient *gclient = nullptr;

    int width, height;

    double currentTime = 0.0;
    std::chrono::time_point<std::chrono::steady_clock> prevTime, suspendTime;

    bool bSession;
    bool bRunning;
    bool bRequestRunning;
    bool bFreezing;

    TimeDate td;

    bool stateKey[512];
    bool shiftStateKey[512];
    bool ctrlStateKey[512];
    bool altStateKey[512];

    double keyAttitudeAccel = ofs::radians(5.0);
    double keyAttitudeBrake = ofs::radians(60.0);
    double keyRotationAccel = ofs::radians(60.0);
    double keyMovementControl = 1000.0;
    double keyMovementBrake = 5.0;

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

extern CoreApp *ofsAppCore;