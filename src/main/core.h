// core.h - main core definitions package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#define APP_NAME     "Orbital Flight Simulator"
#define APP_SHORT    "OFS"

// Default screen coordination size
#define SCR_WIDTH       1920
#define SCR_HEIGHT      1080
#define SCR_FOV         45.0

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>


#include <fmt/printf.h>

typedef const char          cchar_t;
typedef std::string         str_t;
typedef const std::string   cstr_t;

namespace fs = std::filesystem;

#define checkAllFlags(flags, mask)  ((flags & (mask)) == (mask))
#define checkAnyFlags(flags, mask)  (flags & (mask))

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

#include "main/date.h"
#include "main/math.h"
#include "utils/color.h"
