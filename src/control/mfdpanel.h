// mfdpanel.h - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

class Panel;

class MFDInstrument
{
public:
    MFDInstrument(const Panel *panel);
    ~MFDInstrument() = default;

private:
    const Panel *panel = nullptr;

};