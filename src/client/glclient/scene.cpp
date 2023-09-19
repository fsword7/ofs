// scene.cpp - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/player.h"
#include "universe/universe.h"
#include "universe/celbody.h"
#include "universe/star.h"

#include "client.h"
// #include "camera.h"
#include "vobject.h"
#include "scene.h"

Scene::Scene(int width, int height)
: shmgr("shaders/gl")
{
    resize(width, height);
}

void Scene::checkErrors()
{
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        logger->debug("OpenGL Error: {}\n", err);
    }
}

void Scene::init(Universe *uv)
{
    // camera = new Camera(width, height);

    universe = uv;

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
    
    glViewport(0, 0, width, height);
}

glm::dvec3 Scene::getAstrocentricPosition(const Object *object, const glm::dvec3 &vpos, int time)
{
    return vpos - object->getuPosition(time);
}

void Scene::update(Player *player)
{
    camera = player->getCamera();

    pixelSize = (2.0 * tan(camera->getFOV() / 2.0)) / camera->getHeight();

    nearStars.clear();
    visibleStars.clear();
    renderList.clear();

    universe->findCloseStars(camera->getGlobalPosition(), 1.0, nearStars);
    for (auto star : nearStars)
    {
        vObject *vstar = getVisualObject(star, true);
    }
}

void Scene::render(Player *player)
{
    // update(player);

    // Clear all framebuffer
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    renderConstellations();
    renderStars(faintestMag);

    glm::dvec3 obs = camera->getGlobalPosition();

    // for (auto sun : nearStars)
    // {
    //     // logger->info("Sun: {}\n", ofsGetObjectName(sun));

    //     if (!sun->hasSolarSystem())
    //         continue;
    //     System *system = sun->getSolarSystem(); 

    //     PlanetarySystem *objects = system->getPlanetarySystem();
    //     FrameTree *tree = objects->getSystemTree();

    //     glm::dvec3 apos = getAstrocentricPosition(sun, obs, now);
    //     glm::dvec3 vpn = camera->getGlobalRotation() * glm::dvec3(0, 0, -1);

    //     buildSystems(tree, apos, vpn, { 0, 0, 0 });
    // }

    renderSystemObjects();
}