// module.cpp - Module Handler package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/module.h"

OFSAPI ModuleHandle ofsLoadModule(const char *name)
{
    std::string path = name;
#ifdef __unix__
    return dlopen(path.c_str(), RTLD_LAZY|RTLD_GLOBAL|RTLD_DEEPBIND);
#elif __MINGW32__
    return dlopen(path.c_str(), RTLD_LAZY|RTLD_GLOBAL);
#else
    static_assert("System does not support library routines - aborted.\n");
    return nullptr;
#endif
}

OFSAPI void ofsUnloadModule(ModuleHandle module)
{
    dlclose(module);
}

OFSAPI void *ofsGetProcAddress(ModuleHandle module, const char *funcName)
{
    return dlsym(module, funcName);
}

OFSAPI char *ofsGetModuleError()
{
    return dlerror();
}

OFSAPI int ofsGetDateInteger(cstr_t &date)
{
    static const char *mstr[12] =
    { 
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    char ms[32];
    int day, month, year;
    int val;

    sscanf(date.c_str(), "%s%d%d", ms, &day, &year);
    for (int month = 0; month < 12; month++)
        if (!strncmp(ms, mstr[month], 3))
            break;

    val = ((year % 100) * 10000) + ((month + 1) * 100) + day;
    return val;
}

OFSAPI ModuleHandle ofsGetModuleHandle()
{
    return nullptr;
}

OFSAPI void ofsInitModule(ModuleHandle handle)
{
    typedef void (*initModule_t)(ModuleHandle);

    initModule_t initModule;

    initModule = (initModule_t)ofsGetProcAddress(handle, "initModule");
    if (initModule != nullptr)
        initModule(handle);
}

OFSAPI void ofsExitModule(ModuleHandle handle)
{
    typedef void (*exitModule_t)(ModuleHandle);

    exitModule_t exitModule;

    exitModule = (exitModule_t)ofsGetProcAddress(handle, "exitModule");
    if (exitModule != nullptr)
        exitModule(handle);
}
