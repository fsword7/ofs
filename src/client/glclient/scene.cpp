// scene.cpp - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#include "main/core.h"
#include "engine/object.h"
#include "glclient.h"
#include "scene.h"

Scene::Scene(int width, int height)
: width(width), height(height)
{ 
    glViewport(0, 0, width, height);
}

void Scene::init()
{
    vobjList.clear();
}

void Scene::render()
{
    // Clear all framebuffer
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


}