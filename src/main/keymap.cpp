// keymap.cpp - Keymap Definition Package
//
// Author:  Tim Stark
// Date:    Feb 24, 2026

#include "main/core.h"
#include "api/ofsapi.h"
#include "main/keymap.h"

struct {
    int defKey;
    cchar_t *keyName;
} pkeyList[] = {
    { ofs::pkey0, "0"},
    { ofs::pkey1, "1"},
    { ofs::pkey2, "2"},
    { ofs::pkey3, "3"},
    { ofs::pkey4, "4"},
    { ofs::pkey5, "5"},
    { ofs::pkey6, "6"},
    { ofs::pkey7, "7"},
    { ofs::pkey8, "8"},
    { ofs::pkey9, "9"},

    { ofs::pkeyA, "A"},
    { ofs::pkeyB, "B"},
    { ofs::pkeyC, "C"},
    { ofs::pkeyD, "D"},
    { ofs::pkeyE, "E"},
    { ofs::pkeyF, "F"},
    { ofs::pkeyG, "G"},
    { ofs::pkeyH, "H"},
    { ofs::pkeyI, "I"},
    { ofs::pkeyJ, "J"},
    { ofs::pkeyK, "K"},
    { ofs::pkeyL, "L"},
    { ofs::pkeyM, "M"},
    { ofs::pkeyN, "N"},
    { ofs::pkeyO, "O"},
    { ofs::pkeyP, "P"},
    { ofs::pkeyQ, "Q"},
    { ofs::pkeyR, "R"},
    { ofs::pkeyS, "S"},
    { ofs::pkeyT, "T"},
    { ofs::pkeyU, "U"},
    { ofs::pkeyV, "V"},
    { ofs::pkeyW, "W"},
    { ofs::pkeyX, "X"},
    { ofs::pkeyY, "Y"},
    { ofs::pkeyZ, "Z"},

    { ofs::pkeyLeftBracket,  "["},
    { ofs::pkeyBackslash,   "\\"},
    { ofs::pkeyRightBracket,"]"},
    { ofs::pkeySpace, "Space"},
    { ofs::pkeyPeriod, "Period"},
    { ofs::pkeyComma, "Comma"},
    { ofs::pkeyMinus, "Minus"},
    { ofs::pkeyEqual, "Equal"},
    { ofs::pkeySemicolon, "Semicolon"},
    { ofs::pkeyApostrophe, "Apostrophe"},
    { ofs::pkeyGraveAccent, "GraveAccent"},
    { ofs::pkeySlash, "Slash"},

    { ofs::pkeyF1, "F1"},
    { ofs::pkeyF2, "F2"},
    { ofs::pkeyF3, "F3"},
    { ofs::pkeyF4, "F4"},
    { ofs::pkeyF5, "F5"},
    { ofs::pkeyF6, "F6"},
    { ofs::pkeyF7, "F7"},
    { ofs::pkeyF8, "F8"},
    { ofs::pkeyF9, "F9"},
    { ofs::pkeyF10, "F10"},
    { ofs::pkeyF11, "F11"},
    { ofs::pkeyF12, "F12"},

    { ofs::pkeyPad0, "Pad0"},
    { ofs::pkeyPad1, "Pad1"},
    { ofs::pkeyPad2, "Pad2"},
    { ofs::pkeyPad3, "Pad3"},
    { ofs::pkeyPad4, "Pad4"},
    { ofs::pkeyPad5, "Pad5"},
    { ofs::pkeyPad6, "Pad6"},
    { ofs::pkeyPad7, "Pad7"},
    { ofs::pkeyPad8, "Pad8"},
    { ofs::pkeyPad9, "Pad9"},
    { ofs::pkeyPadDecimal, "PadDecimal"},
    { ofs::pkeyPadAdd, "PadAdd"},
    { ofs::pkeyPadSubtract, "PadSubtract"},
    { ofs::pkeyPadMultiply, "PadMultiply"},
    { ofs::pkeyPadDivide, "PadDivide"},
    { ofs::pkeyPadEnter, "PadEnter"},

    { ofs::pkeyLShift, "LeftShift"},
    { ofs::pkeyLControl, "LeftControl"},
    { ofs::pkeyLAlt, "LeftAlt"},
    { ofs::pkeyRShift, "RightShift"},
    { ofs::pkeyRControl, "RightControl"},
    { ofs::pkeyRAlt, "RightAlt"},

    { ofs::pkeyTab, "Tab"},
    { ofs::pkeyBackspace, "Backspace"},
    { ofs::pkeyEnter, "Enter"},
    { ofs::pkeyEscape, "Escape"},

    { ofs::pkeyCapsLock, "CapsLock"},
    { ofs::pkeyScrollLock, "ScrollLock"},
    { ofs::pkeyNumLock, "NumLock"},
    { ofs::pkeyPrintScreen, "PrintScreen"},
    { ofs::pkeyPause, "Pause"},

    { ofs::pkeyUp, "Up"},
    { ofs::pkeyDown, "Down"},
    { ofs::pkeyLeft, "Left"},
    { ofs::pkeyRight, "Right"},
    { ofs::pkeyHome, "Home"},
    { ofs::pkeyEnd, "End"}, 
    { ofs::pkeyPageDown, "PageDown"},
    { ofs::pkeyPageUp, "PageUp"},
    { ofs::pkeyInsert, "Insert"},
    { ofs::pkeyDelete, "Delete"}

 };

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

#ifdef __linux__
    { ofs::lkeyTogglePanelMode,         ofs::pkey8,                         "Toggle-Panel-Mode"},
#else
    { ofs::lkeyTogglePanelMode,         ofs::pkeyF8,                        "Toggle-Panel-Mode"},
#endif
    { ofs::lkeyToggleHUDMode,           ofs::pkeyH|KEYM_CTRL,               "Toggle-HUD-Mode"},
    { ofs::lkeySwitchHUDMode,           ofs::pkeyH,                         "Switch-HUD-Mode"},

#ifdef __linux__
    { ofs::lkeyIncWarpTime,             ofs::pkey4,                        "Increase-Warp-Time"},
    { ofs::lkeyDecWarpTime,             ofs::pkey3,                        "Decrease-Warp-Time"},
    { ofs::lkeyResetWarpTime,           ofs::pkey2,                        "Reset-Warp-Time"},
#else
    { ofs::lkeyIncWarpTime,             ofs::pkeyF4,                        "Increase-Warp-Time"},
    { ofs::lkeyDecWarpTime,             ofs::pkeyF3,                        "Decrease-Warp-Time"},
    { ofs::lkeyResetWarpTime,           ofs::pkeyF2,                        "Reset-Warp-Time"},
#endif
    { ofs::lkeyIncMainThrust,           ofs::pkeyPadAdd,                     "Increase-Main-Thrust"},
    { ofs::lkeyDecMainThrust,           ofs::pkeyPadSubtract,                "Decrease-Main-Thrust"},
    { ofs::lkeyFullMainThrust,          ofs::pkeyPadAdd|KEYM_CTRL,           "Full-Main-Thrust"},
    { ofs::lkeyFullRetroThrust,         ofs::pkeyPadSubtract|KEYM_CTRL,      "Full-Retro-Thrust"},
    { ofs::lkeyBoostMainThrust,         ofs::pkeyPadAdd|KEYM_ALT,            "Boost-Main-Thrust"},
    { ofs::lkeyBoostRetroThrust,        ofs::pkeyPadSubtract|KEYM_ALT,       "Boost-Retro-Thrust"},
    { ofs::lkeyKillMainThrust,          ofs::pkeyPadMultiply,                "Kill-Main-Thrust"},
    { ofs::lkeyIncHoverThrust,          ofs::pkeyPad0,                       "Increase-Hover-Thrust"},
    { ofs::lkeyDecHoverThrust,          ofs::pkeyPadDecimal,                 "Decrease-Hover-Thrust"},

    { ofs::lkeyEnableRCSMode,           ofs::pkeySlash|KEYM_CTRL,            "Enable-RCS-Mode"},
    { ofs::lkeyToggleRCSMode,           ofs::pkeySlash,                      "Toggle-RCS-Mode"},

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

    if ((kmod = lkey & KEYM_SHIFT)) {
        if (kmod == KEYM_SHIFT && !KEYDOWN_SHIFT(keyState))
            return false;
        else if (kmod == KEYM_LSHIFT && !KEYDOWN_LSHIFT(keyState))
            return false;
        else if (kmod == KEYM_RSHIFT && !KEYDOWN_RSHIFT(keyState))
            return false;
    } else if (KEYDOWN_SHIFT(keyState))
        return false;

    if ((kmod = lkey & KEYM_CTRL)) {
        if (kmod == KEYM_CTRL && !KEYDOWN_CTRL(keyState))
            return false;
        else if (kmod == KEYM_LCTRL && !KEYDOWN_LCTRL(keyState))
            return false;
        else if (kmod == KEYM_RCTRL && !KEYDOWN_RCTRL(keyState))
            return false;
    } else if (KEYDOWN_CTRL(keyState))
        return false;

    if ((kmod = lkey & KEYM_ALT)) {
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