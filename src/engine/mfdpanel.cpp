// mfdpanel.cpp - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Aug 28, 2022

#include "main/core.h"
#include "engine/panel.h"
#include "engine/mfdpanel.h"

MFDInstrument::MFDInstrument(const Panel *panel)
: panel(panel)
{
    
}