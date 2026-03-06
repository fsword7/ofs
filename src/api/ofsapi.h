// ofsapi.h - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#pragma once

#ifdef __WIN32__

#define LIBEXPORT   __declspec(dllexport)
#define LIBIMPORT   /* __declspec(dllimport) */
#define LIBCALL     extern "C" LIBEXPORT

#ifdef OFSAPI_SERVER_BUILD
#define OFSAPI LIBEXPORT // server side
#else
#define OFSAPI LIBIMPORT // client side
#endif /* OFSAPI_SERVER_BUILD */

#else /* __WIN32__ */

#define LIBEXPORT
#define LIBIMPORT
#define LIBCALL     extern "C"
#define OFSAPI

#endif /* __WIN32__ */


typedef void * ObjectHandle;

class ofsHandler2;
class Constellations;
class StarDatabase;
class System;
class GUIManager;

enum ObjectType
{
    objUnknown = 0,
    objPlayer,
    objVehicle,
    objSurfBase,
    objCelestial,
    objCelestialStar,
    objCelestialBody,
};

#define SURF_MIPMAPS        0x0001
#define SURF_NOMIPMAPS      0x0002
#define SURF_ALPHA          0x0004
#define SURF_NOALPHA        0x0008

namespace ofs {

    enum pkeyCode : uint8_t
    {
        pkeyUndefined    = 0x00,

        pkeySpace        = 0x20,
        pkeyPeriod       = 0x23,
        pkeyComma        = 0x24,
        pkeyMinus        = 0x25,
        pkeyEqual        = 0x26,
        pkeySemicolon    = 0x27,
        pkeyApostrophe   = 0x28,
        pkeyGraveAccent  = 0x29,

        pkeySlash        = 0x2F,

        pkey0            = 0x30,
        pkey1            = 0x31,
        pkey2            = 0x32,
        pkey3            = 0x33,
        pkey4            = 0x34,
        pkey5            = 0x35,
        pkey6            = 0x36,
        pkey7            = 0x37,
        pkey8            = 0x38,
        pkey9            = 0x39,

        pkeyA            = 0x41,
        pkeyB            = 0x42,
        pkeyC            = 0x43,
        pkeyD            = 0x44,
        pkeyE            = 0x45,
        pkeyF            = 0x46,
        pkeyG            = 0x47,
        pkeyH            = 0x48,
        pkeyI            = 0x49,
        pkeyJ            = 0x4A,
        pkeyK            = 0x4B,
        pkeyL            = 0x4C,
        pkeyM            = 0x4D,
        pkeyN            = 0x4E,
        pkeyO            = 0x4F,
        pkeyP            = 0x50,
        pkeyQ            = 0x51,
        pkeyR            = 0x52,
        pkeyS            = 0x53,
        pkeyT            = 0x54,
        pkeyU            = 0x55,
        pkeyV            = 0x56,
        pkeyW            = 0x57,
        pkeyX            = 0x58,
        pkeyY            = 0x59,
        pkeyZ            = 0x5A,

        pkaya            = 0x61,
        pkeyb            = 0x62,
        pkeyc            = 0x63,
        pkeyd            = 0x64,
        pkeye            = 0x65,
        pkeyf            = 0x66,
        pkeyg            = 0x67,
        pkeyh            = 0x68,
        pkeyi            = 0x69,
        pkeyj            = 0x6A,
        pkeyk            = 0x6B,
        pkeyl            = 0x6C,
        pkeym            = 0x6D,
        pkeyn            = 0x6E,
        pkeyo            = 0x6F,
        pkeyp            = 0x70,
        pkeyq            = 0x71,
        pkeyr            = 0x72,
        pkeys            = 0x73,
        pkeyt            = 0x74,
        pkeyu            = 0x75,
        pkeyv            = 0x76,
        pkeyw            = 0x77,
        pkeyx            = 0x78,
        pkeyy            = 0x79,
        pkeyz            = 0x7A,

        pkeyLeftBracket  = 0x5B,
        pkeyBackslash    = 0x5C,
        pkeyRightBracket = 0x5D,

        pkeyF1           = 0xF1,
        pkeyF2           = 0xF2,
        pkeyF3           = 0xF3,
        pkeyF4           = 0xF4,
        pkeyF5           = 0xF5,
        pkeyF6           = 0xF6,
        pkeyF7           = 0xF7,
        pkeyF8           = 0xF8,
        pkeyF9           = 0xF9,
        pkeyF10          = 0xFA,
        pkeyF11          = 0xFB,
        pkeyF12          = 0xFC,

        pkeyPad0         = 0xE0,
        pkeyPad1         = 0xE1,
        pkeyPad2         = 0xE2,
        pkeyPad3         = 0xE3,
        pkeyPad4         = 0xE4,
        pkeyPad5         = 0xE5,
        pkeyPad6         = 0xE6,
        pkeyPad7         = 0xE7,
        pkeyPad8         = 0xE8,
        pkeyPad9         = 0xE9,
        pkeyPadDecimal   = 0xEA,
        pkeyPadDivide    = 0xEB,
        pkeyPadMultiply  = 0xEC,
        pkeyPadAdd       = 0xED,
        pkeyPadSubtract  = 0xEE,
        pkeyPadEnter     = 0xEF,

        pkeyLShift       = 0xC0,
        pkeyLControl     = 0xC1,
        pkeyLAlt         = 0xC2,
        pkeyRShift       = 0xC3,
        pkeyRControl     = 0xC4,
        pkeyRAlt         = 0xC5,

        pkeyTab          = 0xC6,
        pkeyBackspace    = 0xC7,
        pkeyEnter        = 0xC8,
        pkeyEscape       = 0xC9,

        pkeyCapsLock     = 0xD0,
        pkeyScrollLock   = 0xD1,
        pkeyNumLock      = 0xD2,
        pkeyPrintScreen  = 0xD3,
        pkeyPause        = 0xD4,

        pkeyUp           = 0xD5,
        pkeyDown         = 0xD6,
        pkeyLeft         = 0xD7,
        pkeyRight        = 0xD8,
        pkeyHome         = 0xD9,
        pkeyEnd          = 0xDA,
        pkeyPageUp       = 0xDB,
        pkeyPageDown     = 0xDC,
        pkeyInsert       = 0xDD,
        pkeyDelete       = 0xDE,

    };

    enum lkeyCode {
        lkeyObserverTurnLeft = 1,
        lkeyObserverTurnRight,
        lkeyObserverTurnUp,
        lkeyObserverTurnDown,
        lkeyObserverTiltLeft,
        lkeyObserverTiltRight,
        lkeyObserverMoveUp,
        lkeyObserverMoveDown,
        lkeyObserverResetHome,

        lkeyTravelTurnLeft,
        lkeyTravelTurnRight,
        lkeyTravelTurnUp,
        lkeyTravelTurnDown,
        lkeyTravelMoveForward,
        lkeyTravelMoveBackward,
        lkeyTravelTiltLeft,
        lkeyTravelTiltRight,
        lkeyTravelBrake,

        lkeyOrbitMoveLeft,
        lkeyOrbitMoveRight,
        lkeyOrbitMoveUp,
        lkeyOrbitMoveDown,
        lkeyDollyMoveForward,
        lkeyDollyMoveBackward,

        lkeyTogglePanelMode,
        lkeyToggleHUDMode,
        lkeySwitchHUDMode,

        lkeyIncWarpTime,
        lkeyDecWarpTime,
        lkeyResetWarpTime,

        lkeyIncMainThrust,
        lkeyDecMainThrust,
        lkeyFullMainThrust,
        lkeyFullRetroThrust,
        lkeyBoostMainThrust,
        lkeyBoostRetroThrust,
        lkeyKillMainThrust,
        lkeyIncHoverThrust,
        lkeyDecHoverThrust,

        lkeyEnableRCSMode,
        lkeyToggleRCSMode,

        lkeyRCSRotPitchUp,
        lkeyRCSRotPitchDown,
        lkeyRCSRotYawLeft,
        lkeyRCSRotYawRight,
        lkeyRCSRotBankLeft,
        lkeyRCSRotBankRight,
        lkeyRCSLinMoveUp,
        lkeyRCSLinMoveDown,
        lkeyRCSLinMoveLeft,
        lkeyRCSLinMoveRight,
        lkeyRCSLinMoveForward,
        lkeyRCSLinMoveBackward,

        lkeyLRCSRotPitchUp,
        lkeyLRCSRotPitchDown,
        lkeyLRCSRotYawLeft,
        lkeyLRCSRotYawRight,
        lkeyLRCSRotBankLeft,
        lkeyLRCSRotBankRight,
        lkeyLRCSLinMoveUp,
        lkeyLRCSLinMoveDown,
        lkeyLRCSLinMoveLeft,
        lkeyLRCSLinMoveRight,
        lkeyLRCSLinMoveForward,
        lkeyLRCSLinMoveBackward,

        lkeyCount
    };
};

#define KEYM_CODE       0x00FF
#define KEYM_LSHIFT     0x0100
#define KEYM_RSHIFT     0x0200
#define KEYM_LCTRL      0x0400
#define KEYM_RCTRL      0x0800
#define KEYM_LALT       0x1000
#define KEYM_RALT       0x2000

#define KEYM_SHIFT      (KEYM_LSHIFT|KEYM_RSHIFT)
#define KEYM_CTRL       (KEYM_LCTRL|KEYM_RCTRL)
#define KEYM_ALT        (KEYM_LALT|KEYM_RALT)
// #define KMOD(x)         ((x) << 8)

#define KEYDOWN_SHIFT(x)    ((x)[ofs::pkeyLShift] || (x)[ofs::pkeyRShift])
#define KEYDOWN_LSHIFT(x)   ((x)[ofs::pkeyLShift])
#define KEYDOWN_RSHIFT(x)   ((x)[ofs::pkeyRShift])

#define KEYDOWN_CTRL(x)     ((x)[ofs::pkeyLControl] || (x)[ofs::pkeyRControl])
#define KEYDOWN_LCTRL(x)    ((x)[ofs::pkeyLControl])
#define KEYDOWN_RCTRL(x)    ((x)[ofs::pkeyRControl])

#define KEYDOWN_ALT(x)      ((x)[ofs::pkeyLAlt] || (x)[ofs::pkeyRAlt])
#define KEYDOWN_LALT(x)     ((x)[ofs::pkeyLAlt])
#define KEYDOWN_RALT(x)     ((x)[ofs::pkeyRAlt])

#define NAVMODE_KILLROT         1
#define NAVMODE_HORZLVL         2
#define NAVMODE_ALTHOLD         3
#define NAVMODE_PROGRADE        4
#define NAVMODE_RETROGRADE      5
#define NAVMODE_NMLPLUS         6
#define NAVMODE_NMLMINUS        7

#define DEF_NAVBIT(x)           (1u << ((x) - 1))
#define NAVBIT_KILLROT          DEF_NAVBIT(NAVMODE_KILLROT)
#define NAVBIT_HORZLVL          DEF_NAVBIT(NAVMODE_HORZLVL)
#define NAVBIT_ALTHOLD          DEF_NAVBIT(NAVMODE_ALTHOLD)
#define NAVBIT_PROGRADE         DEF_NAVBIT(NAVMODE_PROGRADE)
#define NAVBIT_RETROGRADE       DEF_NAVBIT(NAVMODE_RETROGRADE)
#define NAVBIT_NMLPLUS          DEF_NAVBIT(NAVMODE_NMLPLUS)
#define NAVBIT_NMLMINUS         DEF_NAVBIT(NAVMODE_NMLMINUS)

class OFSAPI GUIElement
{
    friend class GUIManager;

public:
    GUIElement(cstr_t &name, const std::type_info &id)
    : name(name), idType(id)
    { }
    virtual ~GUIElement() = default;

    inline cstr_t getsName() const  { return name; }
    inline bool isVisible() const   { return bEnabled; }
    inline void enable(bool set)    { bEnabled = set; }

    virtual void show() = 0;

protected:
    cstr_t name;
    const std::type_info &idType;
    bool bEnabled = false;
};

// OFSAPI int ofsGetCameraHeight();
// OFSAPI int ofsGetCameraWidth();

// OFSAPI glm::dvec3 ofsGetCameraGlobalPosition();
// OFSAPI glm::dvec3 ofsGetCameraGlobalDirection();
// OFSAPI glm::dmat3 ofsGetCameraGlobalRotation();
// OFSAPI double ofsGetCameraFieldOfView();
// OFSAPI double ofsGetCameraAspectRatio();
// OFSAPI double ofsGetCameraTanAperature();

// OFSAPI glm::dmat3 ofsGetCameraRotationMatrix();
// OFSAPI glm::dmat4 ofsGetCameraViewMatrix();
// OFSAPI glm::dmat4 ofsGetCameraProjectionMatrix();
// OFSAPI glm::dmat4 ofsGetCameraViewProjMatrix();

// OFSAPI ObjectHandle ofsGetObjectByName(cstr_t &name);
// OFSAPI cstr_t ofsGetObjectName(ObjectHandle object);
// OFSAPI ObjectType ofsGetObjectType(ObjectHandle object);
// OFSAPI double ofsGetObjectRadius(ObjectHandle object);
// OFSAPI glm::dvec3 ofsGetObjectGlobalPosition(ObjectHandle object, int time);

// OFSAPI int ofsGetObjectStarHIPNumber(ObjectHandle object);
// OFSAPI glm::dvec3 ofsGetObjectStarPosition(ObjectHandle object);
// OFSAPI double ofsGetObjectStarTemperature(ObjectHandle object);

// OFSAPI bool ofsStarHasSolarSystem(ObjectHandle object);
// OFSAPI System *ofsStarGetSolarSystem(ObjectHandle object);

// OFSAPI Constellations &ofsGetConstellations();
// OFSAPI StarDatabase &ofsGetStarDatabase();
// OFSAPI void ofsFindClosestStars(const glm::dvec3 &obs, const double dist,
//     std::vector<ObjectHandle> &nearStars);
// OFSAPI void ofsFindVisibleStars(ofsHandler2 &handler,
//     const glm::dvec3 &obs, const glm::dmat3 &rot,
//     const double fov, const double aspect,
//     const double faintest);