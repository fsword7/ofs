// ofsapi.cpp - OFS API interface package
//
// Author:  Tim Stark
// Date:    Sep 2, 2022

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/handle.h"
#include "engine/object.h"
#include "main/app.h"
#include "universe/universe.h"
#include "universe/body.h"
#include "universe/star.h"

// LIBEXPORT int ofsGetCameraHeight()
// {
//     return ofsAppCore->getCamera()->getHeight();
// }

// LIBEXPORT int ofsGetCameraWidth()
// {
//     return ofsAppCore->getCamera()->getWidth();
// }

// LIBEXPORT glm::dvec3 ofsGetCameraGlobalPosition()
// {
//     return ofsAppCore->getCamera()->getGlobalPosition();
// }

// LIBEXPORT glm::dvec3 ofsGetCameraGlobalDirection()
// {
//     return ofsAppCore->getCamera()->getGlobalDirection();
// }

// LIBEXPORT glm::dmat3 ofsGetCameraGlobalRotation()
// {
//     return ofsAppCore->getCamera()->getGlobalRotation();
// }

// LIBEXPORT glm::dmat3 ofsGetCameraRotationMatrix()
// {
//     return ofsAppCore->getCamera()->getGlobalRotation();
// }

// LIBEXPORT double ofsGetCameraFieldOfView()
// {
//     return ofsAppCore->getCamera()->getFOV();
// }

// LIBEXPORT double ofsGetCameraAspectRatio()
// {
//     return ofsAppCore->getCamera()->getAspect();
// }

// LIBEXPORT double ofsGetCameraTanAperature()
// {
//     return ofsAppCore->getCamera()->getTanAp();
// }

// LIBEXPORT glm::dmat4 ofsGetCameraViewMatrix()
// {
//     return ofsAppCore->getCamera()->getViewMatrix();
// }

// LIBEXPORT glm::dmat4 ofsGetCameraProjectionMatrix()
// {
//     return ofsAppCore->getCamera()->getProjectionMatrix();
// }

// LIBEXPORT glm::dmat4 ofsGetCameraViewProjMatrix()
// {
//     return glm::dmat4(1);
// }



// LIBEXPORT ObjectHandle ofsGetObjectByName(cstr_t &name)
// {
//     return ofsAppCore->getUniverse()->findPath(name);
// }

// LIBEXPORT cstr_t ofsGetObjectName(ObjectHandle object)
// {
//     return static_cast<Object *>(object)->getName();
// }

// LIBEXPORT ObjectType ofsGetObjectType(ObjectHandle object)
// {
//     return static_cast<Object *>(object)->getType();
// }

LIBEXPORT double ofsGetObjectRadius(ObjectHandle object)
{
    return static_cast<Object *>(object)->getRadius(); 
}

// LIBEXPORT glm::dvec3 ofsGetObjectGlobalPosition(ObjectHandle object, int time)
// {
//     return static_cast<Object *>(object)->getuPosition(time);
// }

LIBEXPORT int ofsGetObjectStarHIPNumber(ObjectHandle object)
{
    return static_cast<CelestialStar *>(object)->getHIPnumber();
}

LIBEXPORT double ofsGetObjectStarTemperature(ObjectHandle object)
{
    return static_cast<CelestialStar *>(object)->getTemperature();
}

LIBEXPORT glm::dvec3 ofsGetObjectStarPosition(ObjectHandle object)
{
    return static_cast<CelestialStar *>(object)->getStarPosition();
}

// LIBEXPORT bool ofsStarHasSolarSystem(ObjectHandle object)
// {
//     return static_cast<CelestialStar *>(object)->hasSolarSystem();
// }

// LIBEXPORT System *ofsStarGetSolarSystem(ObjectHandle object)
// {
//     return static_cast<CelestialStar *>(object)->getSolarSystem();
// }

LIBEXPORT Constellations &ofsGetConstellations()
{
    return ofsAppCore->getUniverse()->getConstellations();
}

LIBEXPORT StarDatabase &ofsGetStarDatabase()
{
    return ofsAppCore->getUniverse()->getStarDatabase();
}

// LIBEXPORT void ofsFindClosestStars(const glm::dvec3 &obs, const double dist,
//     std::vector<ObjectHandle> &nearStars)
// {
//     ofsAppCore->getUniverse()->findCloseStars(obs, dist, nearStars);
// }

LIBEXPORT void ofsFindVisibleStars(ofsHandler2 &handler,
    const glm::dvec3 &obs, const glm::dmat3 &rot,
    const double fov, const double aspect,
    const double faintest)
{
    ofsAppCore->getUniverse()->findVisibleStars(handler, obs, rot, fov, aspect, faintest);
}
