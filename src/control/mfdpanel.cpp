// mfdpanel.cpp - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "control/panel.h"
#include "control/mfdpanel.h"

MFDInstrument::MFDInstrument(const Panel *panel)
: panel(panel)
{
    
}