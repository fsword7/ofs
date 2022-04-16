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

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>


#include <fmt/printf.h>

typedef const char          cchar_t;
typedef std::string         str_t;
typedef const std::string   cstr_t;

namespace fs = std::filesystem;

