// ofsapi.h - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#pragma once

#ifdef __WIN32__

#define LIBEXPORT   __declspec(dllexport)
#define LIBIMPORT   __declspec(dllimport)
#define LIBCALL     extern "C" LIBEXPORT

#ifdef OFSAPI_SERVER_BUILD
#define OFSAPI LIBEXPORT
#else
#define OFSAPI LIBIMPORT
#endif /* OFSAPI_SERVER_BUILD */

#else /* __WIN32__ */

#define DLLEXPORT
#define DLLIMPORT
#define OFSAPI

#endif /* __WIN32__ */
