// module.cpp - Space Plane 1 Vessel module package
//
// Author:  Tim Stark
// Date:    Mar 10, 2024

#include "main/core.h"
#include "api/vehicle.h"
#include "vehicles/sp1/module.h"

Logger *sp1Logger = nullptr;

LIBCALL void initModule(ModuleHandle handle)
{
    sp1Logger = new Logger(Logger::logDebug, "sp1vessel.log");
    sp1Logger->info("--------- Space Plane 1 Vessel --------\n");

    // gclient = new glClient(handle);
    // if (!ofsRegisterGraphicsClient(gclient))
    // {
    //     printf("Can't register graphics client - aborted.\n");
    //     delete gclient, glLogger;
    //     gclient = nullptr;
    //     glLogger = nullptr;
    // }
}

LIBCALL void exitModule(ModuleHandle handle)
{
    // if (gclient != nullptr)
    // {
    //     ofsUnregisterGraphicsClient(gclient);
    //     delete gclient, glLogger;
    //     gclient = nullptr;
    //     glLogger = nullptr;
    // }
}

LIBCALL VehicleModule *ovcInit(Vehicle *vehicle)
{
    return nullptr;
}

LIBCALL void ovcExit(Vehicle *vehicle)
{
}

// static void __attribute__ ((constructor)) setupModule(void)
// {
//     printf("OpenGL client module loaded.\n");

//     Dl_info info;
//     int ret = dladdr((void *)setupModule, &info);
//     if (ret == 0)
//     {
//         printf("dladdr failed - aborted\n");
//         assert(false);
//     }
//     // void (*initModule)(ModuleHandle) =
//     //     (void(*)(ModuleHandle))ofsGetProcAddress(info.dli_fbase, "initModule");
//     // printf("initModule - %p (%p)\n", initModule, info.dli_fbase);
//     myHandle = info.dli_fbase;
//     initModule(myHandle);
// }

// static void __attribute__ ((destructor)) destroyModule(void)
// {
//     exitModule(myHandle);
//     printf("Space Plane 1 vessel module unloaded.\n");
// }
