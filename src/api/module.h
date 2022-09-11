// module.h - Module API package
//
// Author:  Tim Stark
// Date:    Aug 29, 2022

#pragma once

#include "api/ofsapi.h"

#ifdef __MINGW32__
#include <dlfcn.h>
#endif /* __MINGW32__ */

typedef void * ModuleHandle;



OFSAPI ModuleHandle ofsLoadModule(const char *name);
OFSAPI void ofsUnloadModule(ModuleHandle module);
// OFSAPI Dl_info *ofsGetModuleInfo(ModuleHandle module);
OFSAPI void *ofsGetProcAddress(ModuleHandle module, const char *funcName);
OFSAPI char *ofsGetModuleError();

OFSAPI int ofsGetDateInteger(cstr_t &date);
OFSAPI ModuleHandle ofsGetModuleHandle();