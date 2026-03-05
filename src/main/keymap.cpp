// keymap.cpp - Keymap Definition Package
//
// Author:  Tim Stark
// Date:    Feb 24, 2026

#include "main/core.h"
#include "api/ofsapi.h"
#include "main/keymap.h"

struct {
    int defLogicalKey;
    int defKey;
    cchar_t *keyName;
} lkeyList[] = {
    // Cockpit Camera Rotation keys
    { ofs::lkeyObserverTurnLeft,        ofs::pkeyLeft,                       "Observer-Turn-Left" },
    { ofs::lkeyObserverTurnRight,       ofs::pkeyRight,                      "Observer-Turn-Right" },
    { ofs::lkeyObserverTurnUp,          ofs::pkeyUp,                         "Observer-Turn-Up" },
    { ofs::lkeyObserverTurnDown,        ofs::pkeyDown,                       "Observer-Turn-Down"},
    { ofs::lkeyObserverTiltLeft,        ofs::pkeyLeft|KEYM_CTRL,             "Observer-Tilt-Left"},
    { ofs::lkeyObserverTiltRight,       ofs::pkeyRight|KEYM_CTRL,            "Observer-Tilt-Right"},
    { ofs::lkeyObserverMoveUp,          ofs::pkeyUp|KEYM_CTRL,               "Observer-Move-Up"},
    { ofs::lkeyObserverMoveDown,        ofs::pkeyDown|KEYM_CTRL,             "Observer-Move-Down"},
    { ofs::lkeyObserverResetHome,       ofs::pkeyHome,                       "Observer-Reset-Home"},

    { ofs::lkeyTravelTurnLeft,          ofs::pkeyPad4,                      "Travel-Turn-Left"},
    { ofs::lkeyTravelTurnRight,         ofs::pkeyPad6,                      "Travel-Turn-Right"},
    { ofs::lkeyTravelTurnUp,            ofs::pkeyPad8,                      "Travel-Turn-Up"},
    { ofs::lkeyTravelTurnDown,          ofs::pkeyPad2,                      "Travel-Turn-Down"},
    { ofs::lkeyTravelTiltLeft,          ofs::pkeyPad7,                      "Travel-Tilt-Left"},
    { ofs::lkeyTravelTiltRight,         ofs::pkeyPad9,                      "Travel-Tilt-Right"},
    { ofs::lkeyTravelMoveForward,       ofs::pkeyPad1,                      "Travel-Move-Forward"},
    { ofs::lkeyTravelMoveBackward,      ofs::pkeyPad3,                      "Travel-Move-Backward"},
    { ofs::lkeyTravelBrake,             ofs::pkeyPad5,                      "Travel-Brake"},

    { ofs::lkeyOrbitMoveLeft,           ofs::pkeyLeft,                      "Orbit-Move-Left"},
    { ofs::lkeyOrbitMoveRight,          ofs::pkeyRight,                     "Orbit-Move-Right"},
    { ofs::lkeyOrbitMoveUp,             ofs::pkeyUp,                        "Orbit-Move-Up"},
    { ofs::lkeyOrbitMoveDown,           ofs::pkeyDown,                      "Orbit-Move-Down"},
    { ofs::lkeyDollyMoveForward,        ofs::pkeyHome,                      "Dolly-Move-Forward"},
    { ofs::lkeyDollyMoveBackward,       ofs::pkeyEnd,                       "Dolly-Move-Backward"},

    { ofs::lkeyTogglePanelMode,         ofs::pkeyF8,                        "Toggle-Panel-Mode"},
    { ofs::lkeyToggleHUDMode,           ofs::pkeyH|KEYM_CTRL,               "Toggle-HUD-Mode"},
    { ofs::lkeySwitchHUDMode,           ofs::pkeyH,                         "Switch-HUD-Mode"},

    { ofs::lkeyIncWarpTime,             ofs::pkeyF4,                        "Increase-Warp-Time"},
    { ofs::lkeyDecWarpTime,             ofs::pkeyF3,                        "Decrease-Warp-Time"},
    { ofs::lkeyResetWarpTime,           ofs::pkeyF2,                        "Reset-Warp-Time"},

    { ofs::lkeyIncMainThrust,           ofs::pkeyPadAdd,                     "Increase-Main-Thrust"},
    { ofs::lkeyDecMainThrust,           ofs::pkeyPadSubtract,                "Decrease-Main-Thrust"},
    { ofs::lkeyFullMainThrust,          ofs::pkeyPadAdd|KEYM_CTRL,           "Full-Main-Thrust"},
    { ofs::lkeyFullRetroThrust,         ofs::pkeyPadSubtract|KEYM_CTRL,      "Full-Retro-Thrust"},
    { ofs::lkeyBoostMainThrust,         ofs::pkeyPadAdd|KEYM_ALT,            "Boost-Main-Thrust"},
    { ofs::lkeyBoostRetroThrust,        ofs::pkeyPadSubtract|KEYM_ALT,       "Boost-Retro-Thrust"},
    { ofs::lkeyKillMainThrust,          ofs::pkeyPadMultiply,                "Kill-Main-Thrust"},
    { ofs::lkeyIncHoverThrust,          ofs::pkeyPad0,                       "Increase-Hover-Thrust"},
    { ofs::lkeyDecHoverThrust,          ofs::pkeyPadDecimal,                 "Decrease-Hover-Thrust"},

    { ofs::lkeyRCSEnable,               ofs::pkeySlash|KEYM_CTRL,            "RCS-Enable"},
    { ofs::lkeyRCSMode,                 ofs::pkeySlash,                      "RCS-Mode"},

    { ofs::lkeyRCSRotPitchUp,           ofs::pkeyPad8,                       "RCS-PitchUp"},
    { ofs::lkeyRCSRotPitchDown,         ofs::pkeyPad2,                       "RCS-PitchDown"},
    { ofs::lkeyRCSRotYawLeft,           ofs::pkeyPad1,                       "RCS-YawLeft"},
    { ofs::lkeyRCSRotYawRight,          ofs::pkeyPad3,                       "RCS-YawRight"},
    { ofs::lkeyRCSRotBankLeft,          ofs::pkeyPad4,                       "RCS-BankLeft"},
    { ofs::lkeyRCSRotBankRight,         ofs::pkeyPad6,                       "RCS-BankRight"},
    { ofs::lkeyRCSLinMoveUp,            ofs::pkeyPad8,                       "RCS-Up"},
    { ofs::lkeyRCSLinMoveDown,          ofs::pkeyPad2,                       "RCS-Down"},
    { ofs::lkeyRCSLinMoveLeft,          ofs::pkeyPad4,                       "RCS-Left"},
    { ofs::lkeyRCSLinMoveRight,         ofs::pkeyPad6,                       "RCS-Right"},
    { ofs::lkeyRCSLinMoveForward,       ofs::pkeyPad1,                       "RCS-Forward"},
    { ofs::lkeyRCSLinMoveBackward,      ofs::pkeyPad3,                       "RCS-Backwrd"},

    { ofs::lkeyLRCSRotPitchUp,          ofs::pkeyPad8|KEYM_CTRL,             "RCS-PitchUp-Lean"},
    { ofs::lkeyLRCSRotPitchDown,        ofs::pkeyPad2|KEYM_CTRL,             "RCS-PitchDown-Lean"},
    { ofs::lkeyLRCSRotYawLeft,          ofs::pkeyPad1|KEYM_CTRL,             "RCS-YawLeft-Lean"},
    { ofs::lkeyLRCSRotYawRight,         ofs::pkeyPad3|KEYM_CTRL,             "RCS-YawRight-Lean"},
    { ofs::lkeyRCSRotBankLeft,          ofs::pkeyPad4|KEYM_CTRL,             "RCS-BankLeft-Lean"},
    { ofs::lkeyLRCSRotBankRight,        ofs::pkeyPad6|KEYM_CTRL,             "RCS-BankRight-Lean"},
    { ofs::lkeyLRCSLinMoveUp,           ofs::pkeyPad8|KEYM_CTRL,             "RCS-Up-Lean"},
    { ofs::lkeyLRCSLinMoveDown,         ofs::pkeyPad2|KEYM_CTRL,             "RCS-Down-Lean"},
    { ofs::lkeyLRCSLinMoveLeft,         ofs::pkeyPad4|KEYM_CTRL,             "RCS-Left-Lean"},
    { ofs::lkeyLRCSLinMoveRight,        ofs::pkeyPad6|KEYM_CTRL,             "RCS-Right-Lean"},
    { ofs::lkeyLRCSLinMoveForward,      ofs::pkeyPad1|KEYM_CTRL,             "RCS-Forward-Lean"},
    { ofs::lkeyLRCSLinMoveBackward,     ofs::pkeyPad3|KEYM_CTRL,             "RCS-Backwrd-LeaN"},

};

Keymap::Keymap()
{
    setDefaultLogicalKeys();
}

void Keymap::setDefaultLogicalKeys()
{
    // Clear all logical key mapping
    for (int idx = 0; idx < ofs::lkeyCount; idx++)
        lkeyFunc[idx] = 0;

    // Assign default logical keys to logical function keys
    for (int idx = 0; idx < ARRAY_SIZE(lkeyList); idx++)
        lkeyFunc[lkeyList[idx].defLogicalKey & KEYM_CODE] = lkeyList[idx].defKey;
}

bool Keymap::checkKeyModifiers(const bool *keyState, uint16_t lkey) const
{
    int kmod;

    if (kmod = lkey & KEYM_SHIFT) {
        if (kmod == KEYM_SHIFT && !KEYDOWN_SHIFT(keyState))
            return false;
        else if (kmod == KEYM_LSHIFT && !KEYDOWN_LSHIFT(keyState))
            return false;
        else if (kmod == KEYM_RSHIFT && !KEYDOWN_RSHIFT(keyState))
            return false;
    } else if (KEYDOWN_SHIFT(keyState))
        return false;

    if (kmod = lkey & KEYM_CTRL) {
        if (kmod == KEYM_CTRL && !KEYDOWN_CTRL(keyState))
            return false;
        else if (kmod == KEYM_LCTRL && !KEYDOWN_LCTRL(keyState))
            return false;
        else if (kmod == KEYM_RCTRL && !KEYDOWN_RCTRL(keyState))
            return false;
    } else if (KEYDOWN_CTRL(keyState))
        return false;

    if (kmod = lkey & KEYM_ALT) {
        if (kmod == KEYM_ALT && !KEYDOWN_ALT(keyState))
            return false;
        else if (kmod == KEYM_LALT && !KEYDOWN_LALT(keyState))
            return false;
        else if (kmod == KEYM_RALT && !KEYDOWN_RALT(keyState))
            return false;
    } else if (KEYDOWN_ALT(keyState))
        return false;

    return true;
}

bool Keymap::isLogicalKey(uint8_t key, const bool *keyState, ofs::lkeyCode lfunc) const
{
    if ((lkeyFunc[lfunc] & KEYM_CODE) != key)
        return false;
    return checkKeyModifiers(keyState, lkeyFunc[lfunc]);
}

bool Keymap::isLogicalKey(const bool *keyState, ofs::lkeyCode lfunc) const
{
    if (!keyState[lkeyFunc[lfunc] & KEYM_CODE])
        return false;
    return checkKeyModifiers(keyState, lkeyFunc[lfunc]);
}