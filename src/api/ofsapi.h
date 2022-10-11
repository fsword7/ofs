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


enum ObjectType
{
    objUnknown = 0,
    objPlayer,
    objVessel,
    objCelestialStar,
    objCelestialBody,
};

OFSAPI glm::dvec3 ofsGetCameraGlobalPosition();
OFSAPI glm::dvec3 ofsGetCameraGlobalDirection();
OFSAPI glm::dmat3 ofsGetCameraRotationMatrix();
OFSAPI glm::dmat4 ofsGetCameraViewNatrix();
OFSAPI glm::dmat4 ofsGetCameraProjectionMatrix();
OFSAPI glm::dmat4 ofsGetCameraViewProjMatrix();

OFSAPI ObjectHandle ofsGetObjectByName(cstr_t &name);
OFSAPI ObjectType ofsGetObjectType(ObjectHandle object);
OFSAPI double ofsGetObjectRadius(ObjectHandle object);
OFSAPI glm::dvec3 ofsGetObjectGlobalPosition(ObjectHandle object);

OFSAPI glm::dvec3 ofsGetObjectStarPosition(ObjectHandle object);
OFSAPI double ofsGetObjectStarTemperature(ObjectHandle object);

OFSAPI void ofsFindClosestStars(std::vector<ObjectHandle *> &nearStars);
OFSAPI void ofsFindVisibleStars(std::vector<ObjectHandle *> &visibleStars);