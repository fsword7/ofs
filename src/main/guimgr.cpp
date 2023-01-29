// guimgr.cpp - GUI manager package
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "main/guimgr.h"

GUIManager::GUIManager(GraphicsClient *gclient)
: gclient(gclient)
{
    // Initialize GLFW interface

    if (!glfwInit())
    {
        printf("OFS: Can't initialize GLFW interface - aborted.\n");
        exit(EXIT_FAILURE);
    }

    window = gclient->cbCreateRenderingWindow();
    if (window == nullptr)
        exit(EXIT_FAILURE);
}

GUIManager::~GUIManager()
{
    glfwTerminate();
}

// Initialize GLFW callbacks for events
void GUIManager::setupCallbacks()
{

}

bool GUIManager::shouldClose()
{
    return glfwWindowShouldClose(window);
}

void GUIManager::pollEvents()
{
    glfwPollEvents();
}