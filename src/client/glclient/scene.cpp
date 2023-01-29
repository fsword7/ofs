// scene.cpp - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#include "main/core.h"
#include "engine/object.h"
#include "universe/universe.h"
#include "universe/body.h"
#include "universe/star.h"

#include "client.h"
#include "camera.h"
#include "vobject.h"
#include "scene.h"

Scene::Scene(int width, int height)
: width(width), height(height),
  shmgr("shaders/gl")
{ 
}

void Scene::checkErrors()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        logger->debug("OpenGL Error: {}\n", err);
    }
}

void Scene::init()
{
    camera = new Camera(width, height);

    vobjList.clear();

    initConstellations();
    initStarRenderer();
}

void Scene::start()
{

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    // ObjectHandle earth = ofsGetObjectByName("Sol/Earth");
    // if (earth != nullptr)
    //     vEarth = addVisualObject(earth);
    // else
    //     printf("Can't find planet Earth for visual object\n");
}

void Scene::resize(int w, int h)
{
    width = w;
    height = h;
    if (camera != nullptr)
        camera->setSize(width, height);
}

void Scene::update()
{
    camera->update();

    pixelSize = (2.0 * tan(ofsGetCameraFieldOfView() / 2.0)) / ofsGetCameraHeight();;

    nearStars.clear();
    visibleStars.clear();

    ofsFindClosestStars(ofsGetCameraGlobalPosition(), 1.0, nearStars);
}

glm::dvec3 Scene::getAstrocentricPosition(ObjectHandle object, const glm::dvec3 &vpos, int time)
{
    return vpos - ofsGetObjectGlobalPosition(object, time);
}

void Scene::render()
{
    update();

    // Clear all framebuffer
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    renderConstellations();
    renderStars(faintestMag);

    glm::dvec3 obs = ofsGetCameraGlobalPosition();

    for (auto sun : nearStars)
    {
        // logger->info("Sun: {}\n", ofsGetObjectName(sun));

        if (!ofsStarHasSolarSystem(sun))
            continue;
        System *system = ofsStarGetSolarSystem(sun);

        PlanetarySystem *objects = system->getPlanetarySystem();
        FrameTree *tree = objects->getSystemTree();

        glm::dvec3 apos = getAstrocentricPosition(sun, obs, now);
        glm::dvec3 vpn = ofsGetCameraRotationMatrix() * glm::dvec3(0, 0, -1);

        buildSystems(tree, apos, vpn, { 0, 0, 0 });
    }

    // vEarth->update();
    // vEarth->render();
}