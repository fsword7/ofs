// ofsapi.h - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#pragma once

#ifdef __WIN32__

#define LIBEXPORT   __declspec(dllexport)
#define LIBIMPORT   /* __declspec(dllimport) */
#define LIBCALL     extern "C" LIBEXPORT

#ifdef OFSAPI_SERVER_BUILD
#define OFSAPI LIBEXPORT // server side
#else
#define OFSAPI LIBIMPORT // client side
#endif /* OFSAPI_SERVER_BUILD */

#else /* __WIN32__ */

#define LIBEXPORT
#define LIBIMPORT
#define LIBCALL     extern "C"
#define OFSAPI

#endif /* __WIN32__ */


typedef void * ObjectHandle;

class ofsHandler2;
class Constellations;
class StarDatabase;
class System;

enum ObjectType
{
    objUnknown = 0,
    objPlayer,
    objVessel,
    objCelestialStar,
    objCelestialBody,
};

#define SURF_MIPMAPS        0x0001
#define SURF_NOMIPMAPS      0x0002
#define SURF_ALPHA          0x0004
#define SURF_NOALPHA        0x0008

// OFSAPI int ofsGetCameraHeight();
// OFSAPI int ofsGetCameraWidth();

// OFSAPI glm::dvec3 ofsGetCameraGlobalPosition();
// OFSAPI glm::dvec3 ofsGetCameraGlobalDirection();
// OFSAPI glm::dmat3 ofsGetCameraGlobalRotation();
// OFSAPI double ofsGetCameraFieldOfView();
// OFSAPI double ofsGetCameraAspectRatio();
// OFSAPI double ofsGetCameraTanAperature();

// OFSAPI glm::dmat3 ofsGetCameraRotationMatrix();
// OFSAPI glm::dmat4 ofsGetCameraViewMatrix();
// OFSAPI glm::dmat4 ofsGetCameraProjectionMatrix();
// OFSAPI glm::dmat4 ofsGetCameraViewProjMatrix();

// OFSAPI ObjectHandle ofsGetObjectByName(cstr_t &name);
// OFSAPI cstr_t ofsGetObjectName(ObjectHandle object);
// OFSAPI ObjectType ofsGetObjectType(ObjectHandle object);
// OFSAPI double ofsGetObjectRadius(ObjectHandle object);
// OFSAPI glm::dvec3 ofsGetObjectGlobalPosition(ObjectHandle object, int time);

// OFSAPI int ofsGetObjectStarHIPNumber(ObjectHandle object);
// OFSAPI glm::dvec3 ofsGetObjectStarPosition(ObjectHandle object);
// OFSAPI double ofsGetObjectStarTemperature(ObjectHandle object);

// OFSAPI bool ofsStarHasSolarSystem(ObjectHandle object);
// OFSAPI System *ofsStarGetSolarSystem(ObjectHandle object);

// OFSAPI Constellations &ofsGetConstellations();
// OFSAPI StarDatabase &ofsGetStarDatabase();
// OFSAPI void ofsFindClosestStars(const glm::dvec3 &obs, const double dist,
//     std::vector<ObjectHandle> &nearStars);
// OFSAPI void ofsFindVisibleStars(ofsHandler2 &handler,
//     const glm::dvec3 &obs, const glm::dmat3 &rot,
//     const double fov, const double aspect,
//     const double faintest);