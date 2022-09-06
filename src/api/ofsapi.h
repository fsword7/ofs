// ofsapi.h - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#pragma once

#ifdef __WIN32__

#define LIBEXPORT   __declspec(dllexport)
#define LIBIMPORT   __declspec(dllimport)
#define LIBCALL     extern "C" LIBEXPORT

#ifdef OFS_IMPLEMENTATION
#define OFSAPI LIBEXPORT
#else
#define OFSAPI LIBIMPORT
#endif /* OFS_IMPLEMENTATION */

#else /* __WIN32__ */

#define DLLEXPORT
#define DLLIMPORT
#define OFSAPI

#endif /* __WIN32__ */
