// keymap.h - Keymap Definition Package
//
// Author:  Tim Stark
// Date:    Feb 24, 2026

#pragma once

class Keymap
{
public:
    Keymap();

    void setDefaultLogicalKeys();

    bool checkKeyModifiers(const bool *keyState, uint16_t lkey) const;
    bool isLogicalKey(uint8_t key, const bool *keyState, ofs::lkeyCode lfunc) const;
    bool isLogicalKey(const bool *keyState, ofs::lkeyCode lfunc) const;

private:
    uint16_t lkeyFunc[ofs::lkeyCount];
};