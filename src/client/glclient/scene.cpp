// scene.cpp - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#include "main/core.h"
#include "engine/object.h"
#include "client.h"
#include "camera.h"
#include "vobject.h"
#include "scene.h"

Scene::Scene(int width, int height)
: width(width), height(height),
  shmgr("shaders/gl")
{ 
}

void Scene::init()
{
    camera = new Camera(width, height);

    vobjList.clear();
}

void Scene::start()
{

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    ObjectHandle earth = ofsGetObjectByName("Sol/Earth");
    if (earth != nullptr)
        vEarth = addVisualObject(earth);
    else
        printf("Can't find planet Earth for visual object\n");
}

void Scene::update()
{
    camera->update();

    nearStars.clear();
    visibleStars.clear();

    ofsFindClosestStars(nearStars);
    ofsFindVisibleStars(visibleStars);
}

void Scene::render()
{
    update();

    // Clear all framebuffer
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    vEarth->render();
}