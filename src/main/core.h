// core.h - main core definitions package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

#define APP_NAME     "Orbital Flight Simulator"
#define APP_SHORT    "OFS"

#define SCR_FOV         45.0

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <algorithm>
#include <typeinfo>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using cjson = const nlohmann::json;

typedef const char          cchar_t;
typedef std::string         str_t;
typedef const std::string   cstr_t;

namespace fs = std::filesystem;

#define checkAllFlags(flags, mask)  ((flags & (mask)) == (mask))
#define checkAnyFlags(flags, mask)  (flags & (mask))

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

#include "api/logger.h"
#include "main/date.h"
#include "main/math.h"
#include "main/timedate.h"
#include "utils/color.h"
#include "utils/string.h"
#include "api/module.h"

class CoreApp;
class TimeDate;

extern CoreApp *ofsAppCore;
extern Logger *ofsLogger;
extern TimeDate *ofsDate;