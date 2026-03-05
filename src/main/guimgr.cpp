// guimgr.cpp - GUI manager package for GLFW interface
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "engine/player.h"
#include "main/app.h"
#include "main/guimgr.h"

IMGUI_API ImGuiContext *GImGui = nullptr;

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

    // Determine primary monitor size
    setupPrimaryMonitorSize();

    video = gclient->getVideoData();
    video->mode = 0; // windowed mode (default)
    video->width = (sWidth * 3) / 4;
    video->height = (sHeight * 3) / 4;

    window = gclient->createRenderingWindow();
    if (window == nullptr)
        exit(EXIT_FAILURE);

    resetKeys();

}

GUIManager::~GUIManager()
{
    glfwTerminate();
}

void GUIManager::resetKeys()
{
    // Clear all key mapping table
    memset(keys, 0, GLFW_KEY_LAST);

    keys[GLFW_KEY_0]                = ofs::pkey0;
    keys[GLFW_KEY_1]                = ofs::pkey1;
    keys[GLFW_KEY_2]                = ofs::pkey2;
    keys[GLFW_KEY_3]                = ofs::pkey3;
    keys[GLFW_KEY_4]                = ofs::pkey4;
    keys[GLFW_KEY_5]                = ofs::pkey5;
    keys[GLFW_KEY_6]                = ofs::pkey6;
    keys[GLFW_KEY_7]                = ofs::pkey7;
    keys[GLFW_KEY_8]                = ofs::pkey8;
    keys[GLFW_KEY_9]                = ofs::pkey9;

    keys[GLFW_KEY_A]                = ofs::pkeyA;
    keys[GLFW_KEY_B]                = ofs::pkeyB;
    keys[GLFW_KEY_C]                = ofs::pkeyC;
    keys[GLFW_KEY_D]                = ofs::pkeyD;
    keys[GLFW_KEY_E]                = ofs::pkeyE;
    keys[GLFW_KEY_F]                = ofs::pkeyF;
    keys[GLFW_KEY_G]                = ofs::pkeyG;
    keys[GLFW_KEY_H]                = ofs::pkeyH;
    keys[GLFW_KEY_I]                = ofs::pkeyI;
    keys[GLFW_KEY_J]                = ofs::pkeyJ;
    keys[GLFW_KEY_K]                = ofs::pkeyK;
    keys[GLFW_KEY_L]                = ofs::pkeyL;
    keys[GLFW_KEY_M]                = ofs::pkeyM;
    keys[GLFW_KEY_N]                = ofs::pkeyN;
    keys[GLFW_KEY_O]                = ofs::pkeyO;
    keys[GLFW_KEY_P]                = ofs::pkeyP;
    keys[GLFW_KEY_Q]                = ofs::pkeyQ;
    keys[GLFW_KEY_R]                = ofs::pkeyR;
    keys[GLFW_KEY_S]                = ofs::pkeyS;
    keys[GLFW_KEY_T]                = ofs::pkeyT;
    keys[GLFW_KEY_U]                = ofs::pkeyU;
    keys[GLFW_KEY_V]                = ofs::pkeyV;
    keys[GLFW_KEY_W]                = ofs::pkeyW;
    keys[GLFW_KEY_X]                = ofs::pkeyX;
    keys[GLFW_KEY_Y]                = ofs::pkeyY;
    keys[GLFW_KEY_Z]                = ofs::pkeyZ;

    keys[GLFW_KEY_LEFT_BRACKET]     = ofs::pkeyLeftBracket;
    keys[GLFW_KEY_RIGHT_BRACKET]    = ofs::pkeyRightBracket;

    keys[GLFW_KEY_SPACE]            = ofs::pkeySpace;
    keys[GLFW_KEY_SLASH]            = ofs::pkeySlash;
    keys[GLFW_KEY_PERIOD]           = ofs::pkeyPeriod;
    keys[GLFW_KEY_COMMA]            = ofs::pkeyComma;
    keys[GLFW_KEY_MINUS]            = ofs::pkeyMinus;
    keys[GLFW_KEY_EQUAL]            = ofs::pkeyEqual;
    keys[GLFW_KEY_SEMICOLON]        = ofs::pkeySemicolon;
    keys[GLFW_KEY_APOSTROPHE]       = ofs::pkeyApostrophe;
    keys[GLFW_KEY_BACKSLASH]        = ofs::pkeyBackslash;
    keys[GLFW_KEY_GRAVE_ACCENT]     = ofs::pkeyGraveAccent;

    keys[GLFW_KEY_LEFT_SHIFT]       = ofs::pkeyLShift;
    keys[GLFW_KEY_LEFT_CONTROL]     = ofs::pkeyLControl;
    keys[GLFW_KEY_LEFT_ALT]         = ofs::pkeyLAlt;
    keys[GLFW_KEY_RIGHT_SHIFT]      = ofs::pkeyRShift;
    keys[GLFW_KEY_RIGHT_CONTROL]    = ofs::pkeyRControl;
    keys[GLFW_KEY_RIGHT_ALT]        = ofs::pkeyRAlt;

    keys[GLFW_KEY_TAB]              = ofs::pkeyTab;
    keys[GLFW_KEY_BACKSPACE]        = ofs::pkeyBackspace;
    keys[GLFW_KEY_ENTER]            = ofs::pkeyEnter;
    keys[GLFW_KEY_ESCAPE]           = ofs::pkeyEscape;

    keys[GLFW_KEY_LEFT]             = ofs::pkeyLeft;
    keys[GLFW_KEY_RIGHT]            = ofs::pkeyRight;
    keys[GLFW_KEY_UP]               = ofs::pkeyUp;
    keys[GLFW_KEY_DOWN]             = ofs::pkeyDown;
    keys[GLFW_KEY_DELETE]           = ofs::pkeyDelete;
    keys[GLFW_KEY_INSERT]           = ofs::pkeyInsert;
    keys[GLFW_KEY_PAGE_UP]          = ofs::pkeyPageUp;
    keys[GLFW_KEY_PAGE_DOWN]        = ofs::pkeyPageDown;
    keys[GLFW_KEY_HOME]             = ofs::pkeyHome;
    keys[GLFW_KEY_END]              = ofs::pkeyEnd;
    keys[GLFW_KEY_CAPS_LOCK]        = ofs::pkeyCapsLock;
    keys[GLFW_KEY_SCROLL_LOCK]      = ofs::pkeyScrollLock;
    keys[GLFW_KEY_NUM_LOCK]         = ofs::pkeyNumLock;
    keys[GLFW_KEY_PRINT_SCREEN]     = ofs::pkeyPrintScreen;
    keys[GLFW_KEY_PAUSE]            = ofs::pkeyPause;

    keys[GLFW_KEY_F1]               = ofs::pkeyF1;
    keys[GLFW_KEY_F2]               = ofs::pkeyF2;
    keys[GLFW_KEY_F3]               = ofs::pkeyF3;
    keys[GLFW_KEY_F4]               = ofs::pkeyF4;
    keys[GLFW_KEY_F5]               = ofs::pkeyF5;
    keys[GLFW_KEY_F6]               = ofs::pkeyF6;
    keys[GLFW_KEY_F7]               = ofs::pkeyF7;
    keys[GLFW_KEY_F8]               = ofs::pkeyF8;
    keys[GLFW_KEY_F9]               = ofs::pkeyF9;
    keys[GLFW_KEY_F10]              = ofs::pkeyF10;
    keys[GLFW_KEY_F11]              = ofs::pkeyF11;
    keys[GLFW_KEY_F12]              = ofs::pkeyF12;

    keys[GLFW_KEY_KP_0]             = ofs::pkeyPad0;
    keys[GLFW_KEY_KP_1]             = ofs::pkeyPad1;
    keys[GLFW_KEY_KP_2]             = ofs::pkeyPad2;
    keys[GLFW_KEY_KP_3]             = ofs::pkeyPad3;
    keys[GLFW_KEY_KP_4]             = ofs::pkeyPad4;
    keys[GLFW_KEY_KP_5]             = ofs::pkeyPad5;
    keys[GLFW_KEY_KP_6]             = ofs::pkeyPad6;
    keys[GLFW_KEY_KP_7]             = ofs::pkeyPad7;
    keys[GLFW_KEY_KP_8]             = ofs::pkeyPad8;
    keys[GLFW_KEY_KP_9]             = ofs::pkeyPad9;
    keys[GLFW_KEY_KP_DECIMAL]       = ofs::pkeyPadDecimal;
    keys[GLFW_KEY_KP_DIVIDE]        = ofs::pkeyPadDivide;
    keys[GLFW_KEY_KP_MULTIPLY]      = ofs::pkeyPadMultiply;
    keys[GLFW_KEY_KP_ADD]           = ofs::pkeyPadAdd;
    keys[GLFW_KEY_KP_SUBTRACT]      = ofs::pkeyPadSubtract;
    keys[GLFW_KEY_KP_ENTER]         = ofs::pkeyPadEnter;

}


void GUIManager::registerControl(GUIElement *ctrl)
{
    for (auto &e : guiControls)
        if (ctrl == e)
            return;
    guiControls.push_back(ctrl);
}

void GUIManager::unregisterControl(GUIElement *ctrl)
{
    for (auto it = guiControls.begin(); it != guiControls.end(); it++)
        if (ctrl == *it) {
            it = guiControls.erase(it);
            return;
        }
}

// template <class T>
// T *GUIManager::getControl()
// {
//     for (auto &e : guiControls)
//         if (e->type == T::etype)
//             return (T *)e;
//     return nullptr;
// }

// template <class T>
// void GUIManager::showControl()
// {
//     auto e = getControl<T>();
//     if (e != nullptr)
//         e->enable(true);
// }

// template <class T>
// void GUIManager::hideControl()
// {
//     auto e = getControl<T>();
//     if (e != nullptr)
//         e->enable(false);
// }

// template <class T>
// void GUIManager::toggleControl()
// {
//     auto e = getControl<T>();
//     if (e != nullptr)
//         e->enable(!e->isVisible());
// }


void GUIManager::toggleFullScreen()
{

}

// Rendering GUI user interface
void GUIManager::render()
{
    gclient->startImGuiNewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto &ctrl : guiControls)
        if (ctrl->isVisible())
            ctrl->show();

    ImGui::Render();
    gclient->renderImGuiDrawData();
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

    float vx = 0.0, vy = 0.0;
    Camera *cam = player->getCamera();
    if (cam != nullptr)
        cam->mapMouse(xpos, ypos, vx, vy);
    // Adjust for aspect ratio
    vx *= cam->getAspect();

    title = fmt::format("{} X: {} Y: {} ({},{})\n", 
        APP_SHORT, xpos, ypos, vx, vy);
    glfwSetWindowTitle(window, title.c_str());

    glm::dvec3 pickRay = cam->getPickRay(vx, vy);

    // glm::dvec3 obs = player->getPosition();
    // glm::dmat3 rot = player->getRotation();
    // pickObject(obs, rot * pickRay);
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

    uint8_t key = keys[gkey];

    // Logger::logger->debug("Code: {} -> {} action: {} mods: {}\n", gkey,  int(keys[gkey]), action, mods);

    // Just ignore repeating action (holding keys down)
    if (action == GLFW_REPEAT)
        return;
    // Process keyboard movement control
    ofsAppCore->keyPress(key, action != GLFW_RELEASE);
}

void GUIManager::setupPrimaryMonitorSize()
{
    GLFWmonitor *primary = glfwGetPrimaryMonitor();

    glfwGetMonitorWorkarea(primary, nullptr, nullptr, &sWidth, &sHeight);
    glfwGetMonitorPhysicalSize(primary, &mmWidth, &mmHeight);
}