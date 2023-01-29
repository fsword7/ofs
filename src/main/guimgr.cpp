// guimgr.cpp - GUI manager package for GLFW interface
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "main/app.h"
#include "engine/camera.h"
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

// Window event callback function calls
static GUIManager *guiManager = nullptr;

static void cbProcessFramebufferSize(GLFWwindow *window, int width, int height)
{
    guiManager->processFramebufferSize(window, width, height);
}

static void cbProcessScroll(GLFWwindow *window, double xoff, double yoff)
{
    guiManager->processScroll(window, xoff, yoff);
}

static void cbProcessCursorPosition(GLFWwindow *window, double xpos, double ypos)
{
    guiManager->processCursorPosition(window, xpos, ypos);
}

static void cbProcessMouseButton(GLFWwindow *window, int button, int action, int mods)
{
    guiManager->processMouseButton(window, button, action, mods);
}

static void cbProcessKey(GLFWwindow *window, int gkey, int scancode, int action, int mods)
{
    guiManager->processKey(window, gkey, scancode, action, mods);
}

// Initialize GLFW callbacks for events
void GUIManager::setupCallbacks()
{
    guiManager = this;

    glfwSetFramebufferSizeCallback(window, cbProcessFramebufferSize);
    glfwSetScrollCallback(window, cbProcessScroll);
    glfwSetCursorPosCallback(window, cbProcessCursorPosition);
    glfwSetMouseButtonCallback(window, cbProcessMouseButton);
    glfwSetKeyCallback(window, cbProcessKey);
}

bool GUIManager::shouldClose()
{
    return glfwWindowShouldClose(window);
}

void GUIManager::pollEvents()
{
    glfwPollEvents();
}


    // bRunningApp = true;
    // while (bRunningApp)
    // {
    //     int mx, my;
    //     int state;
    //     uint16_t mod;
    //     str_t title;
        
    //     while (SDL_PollEvent(&event))
    //     {
    //         switch(event.type)
    //         {
    //         case SDL_QUIT:
    //             bRunningApp = false;
    //             break;

    //         // Handling keyboard events
    //         case SDL_KEYDOWN:
    //             mod = event.key.keysym.mod;
    //             // processKeyEvent(&event.key, true);
    //             break;

    //         case SDL_KEYUP:
    //             mod = event.key.keysym.mod;
    //             // processKeyEvent(&event.key, false);
    //             break;

    //         // Handling mouse events
    //         case SDL_MOUSEMOTION:
    //             mx = event.motion.x;
    //             my = event.motion.y;

    //             state = 0;
    //             if (event.motion.state & SDL_BUTTON_LMASK)
    //                 state |= mouseLeftButton;
    //             if (event.motion.state & SDL_BUTTON_MMASK)
    //                 state |= mouseMiddleButton;
    //             if (event.motion.state & SDL_BUTTON_RMASK)
    //                 state |= mouseRightButton;
    //             if (mod & KMOD_CTRL)
    //                 state |= mouseControlButton;
    //             if (mod & KMOD_ALT)
    //                 state |= mouseAltButton;
    //             if (mod & KMOD_SHIFT)
    //                 state |= mouseShiftButton;

    //             // if (activeView != nullptr)
    //             // {
    //             //     activeView->map(mx / float(width), my / float(height), vx, vy);
    //             //     pickRay = player->getPickRay(vx, vy);
    //             // }

    //             title = fmt::format("{} X: {} Y {} ({},{}) State: {}{}{}{}{}{}\n",
    //                 APP_SHORT, mx, my, 0, 0, /* pickRay.x(), pickRay.y(), */
    //                 (state & mouseLeftButton    ? 'L' : '-'),
    //                 (state & mouseMiddleButton  ? 'M' : '-'),
    //                 (state & mouseRightButton   ? 'R' : '-'),
    //                 (state & mouseControlButton ? 'C' : '-'),
    //                 (state & mouseAltButton     ? 'A' : '-'),
    //                 (state & mouseShiftButton   ? 'S' : '-'));
    //             setWindowTitle(title);

    //             camera->mouseMove(mx, my, state);
    //             break;

    //         case SDL_MOUSEBUTTONDOWN:
    //             mx = event.motion.x;
    //             my = event.motion.y;

    //             state = 0;
    //             if (event.motion.state & SDL_BUTTON_LMASK)
    //                 state |= mouseLeftButton;
    //             if (event.motion.state & SDL_BUTTON_MMASK)
    //                 state |= mouseMiddleButton;
    //             if (event.motion.state & SDL_BUTTON_RMASK)
    //                 state |= mouseRightButton;
    //             if (mod & KMOD_CTRL)
    //                 state |= mouseControlButton;
    //             if (mod & KMOD_ALT)
    //                 state |= mouseAltButton;
    //             if (mod & KMOD_SHIFT)
    //                 state |= mouseShiftButton;

    //             camera->mousePressButtonDown(mx, my, state);
    //             break;

    //         case SDL_MOUSEBUTTONUP:
    //             mx = event.motion.x;
    //             my = event.motion.y;

    //             state = 0;
    //             if (event.motion.state & SDL_BUTTON_LMASK)
    //                 state |= mouseLeftButton;
    //             if (event.motion.state & SDL_BUTTON_MMASK)
    //                 state |= mouseMiddleButton;
    //             if (event.motion.state & SDL_BUTTON_RMASK)
    //                 state |= mouseRightButton;
    //             if (mod & KMOD_CTRL)
    //                 state |= mouseControlButton;
    //             if (mod & KMOD_ALT)
    //                 state |= mouseAltButton;
    //             if (mod & KMOD_SHIFT)
    //                 state |= mouseShiftButton;

    //             camera->mousePressButtonUp(mx, my, state);
    //             break;

    //         case SDL_MOUSEWHEEL:
    //             if (event.wheel.y > 0)      // scroll up
    //                 camera->mouseDialWheel(-1.0f, 0);
    //             else if (event.wheel.y < 0) // scroll down
    //                 camera->mouseDialWheel(1.0f, 0);
    //             break;
           
                 
    //         }
    //     }
    // }

void GUIManager::processFramebufferSize(GLFWwindow *window, int width, int height)
{
    Camera *cam = ofsAppCore->getCamera();
    if (cam != nullptr)
        cam->resize(width, height);
    gclient->setViewportSize(width, height);
}

void GUIManager::processScroll(GLFWwindow *window, double xoff, double yoff)
{
}

void GUIManager::processCursorPosition(GLFWwindow *window, double xpos, double ypos)
{
    std::string title;

    // title = fmt::format("{} X: {} Y {} ({},{}) State: {}{}{}{}{}{}\n",
    //     APP_SHORT, xpos, ypos, 0, 0, /* pickRay.x(), pickRay.y(), */
    //     (state & mouseLeftButton    ? 'L' : '-'),
    //     (state & mouseMiddleButton  ? 'M' : '-'),
    //     (state & mouseRightButton   ? 'R' : '-'),
    //     (state & mouseControlButton ? 'C' : '-'),
    //     (state & mouseAltButton     ? 'A' : '-'),
    //     (state & mouseShiftButton   ? 'S' : '-'));
    // setWindowTitle(title);

    title = fmt::format("{} X: {} Y: {}\n", APP_SHORT, xpos, ypos);
    glfwSetWindowTitle(window, title.c_str());
}

void GUIManager::processMouseButton(GLFWwindow *window, int button, int action, int mods)
{
}

void GUIManager::processKey(GLFWwindow *window, int gkey, int scancode, int action, int mods)
{
}
