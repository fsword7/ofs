// guimgr.cpp - GUI manager package
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include "main/core.h"
#include "main/guimgr.h"

void GUIManager::init()
{
    if (glfwInit() != GLFW_TRUE)
    {
        printf("OFS: Can't initialize GLFW v3 interface - aborted.\n");
        abort();
    }

}

void GUIManager::cleanup()
{
    glfwTerminate();
}