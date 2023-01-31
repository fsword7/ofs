// guimgr.cpp - GUI manager package for GLFW interface
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "main/app.h"
#include "engine/camera.h"
#include "main/guimgr.h"

// LIBEXPORT *GImGui = nullptr;

GUIManager::GUIManager(GraphicsClient *gclient)
: gclient(gclient)
{
    // Initialize GLFW interface

    if (!glfwInit())
    {
        printf("OFS: Can't initialize GLFW interface - aborted.\n");
        exit(EXIT_FAILURE);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    window = gclient->cbCreateRenderingWindow();
    if (window == nullptr)
        exit(EXIT_FAILURE);

    // Clear all key mapping table
    memset(keys, 0, GLFW_KEY_LAST);
}

GUIManager::~GUIManager()
{
    glfwTerminate();
}

// Rendering GUI user interface
void GUIManager::render()
{

}

// Window event callback function calls
static GUIManager *guiManager = nullptr;

static void cbProcessFramebufferSize(GLFWwindow *window, int width, int height)
{
    guiManager->processFramebufferSize(window, width, height);
}

static void cbProcessMouseWheel(GLFWwindow *window, double xoff, double yoff)
{
    guiManager->processMouseWheel(window, xoff, yoff);
}

static void cbProcessMousePosition(GLFWwindow *window, double xpos, double ypos)
{
    guiManager->processMousePosition(window, xpos, ypos);
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
    pcbProcessMouseWheel = glfwSetScrollCallback(window, cbProcessMouseWheel);
    pcbProcessMousePosition = glfwSetCursorPosCallback(window, cbProcessMousePosition);
    pcbProcessMouseButton = glfwSetMouseButtonCallback(window, cbProcessMouseButton);
    pcbProcessKey = glfwSetKeyCallback(window, cbProcessKey);
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

void GUIManager::processMouseWheel(GLFWwindow *window, double xoff, double yoff)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += xoff;
    io.MouseWheel += yoff;
    if (pcbProcessMouseWheel != nullptr)
        pcbProcessMouseWheel(window, xoff, yoff);
    if (io.WantCaptureMouse)
        return;

}

void GUIManager::processMousePosition(GLFWwindow *window, double xpos, double ypos)
{
    ImGuiIO &io = ImGui::GetIO();
    if (pcbProcessMousePosition != nullptr)
        pcbProcessMousePosition(window, xpos, ypos);
    if (io.WantCaptureMouse)
        return;

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
    ImGuiIO &io = ImGui::GetIO();
    if (pcbProcessMouseButton != nullptr)
        pcbProcessMouseButton(window, button, action, mods);
    if (io.WantCaptureMouse)
        return;
}

void GUIManager::processKey(GLFWwindow *window, int gkey, int scancode, int action, int mods)
{    
    ImGuiIO &io = ImGui::GetIO();
    if (pcbProcessKey != nullptr)
        pcbProcessKey(window, gkey, scancode, action, mods);
    if (io.WantCaptureKeyboard)
        return;
    if (gkey == GLFW_KEY_UNKNOWN)
        return;
    
    // Process keyboard movement control

    // Just ignore repeating action
    if (action == GLFW_REPEAT)
        return;
    // Process pressed keys

}
