// guimgr.cpp - GUI manager package for GLFW interface
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include "main/core.h"
#include "api/graphics.h"
#include "main/keys.h"
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

    window = gclient->cbCreateRenderingWindow();
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

    keys[GLFW_KEY_0]                = ofs::keyCode::key0;
    keys[GLFW_KEY_1]                = ofs::keyCode::key1;
    keys[GLFW_KEY_2]                = ofs::keyCode::key2;
    keys[GLFW_KEY_3]                = ofs::keyCode::key3;
    keys[GLFW_KEY_4]                = ofs::keyCode::key4;
    keys[GLFW_KEY_5]                = ofs::keyCode::key5;
    keys[GLFW_KEY_6]                = ofs::keyCode::key6;
    keys[GLFW_KEY_7]                = ofs::keyCode::key7;
    keys[GLFW_KEY_8]                = ofs::keyCode::key8;
    keys[GLFW_KEY_9]                = ofs::keyCode::key9;

    keys[GLFW_KEY_A]                = ofs::keyCode::keyA;
    keys[GLFW_KEY_B]                = ofs::keyCode::keyB;
    keys[GLFW_KEY_C]                = ofs::keyCode::keyC;
    keys[GLFW_KEY_D]                = ofs::keyCode::keyD;
    keys[GLFW_KEY_E]                = ofs::keyCode::keyE;
    keys[GLFW_KEY_F]                = ofs::keyCode::keyF;
    keys[GLFW_KEY_G]                = ofs::keyCode::keyG;
    keys[GLFW_KEY_H]                = ofs::keyCode::keyH;
    keys[GLFW_KEY_I]                = ofs::keyCode::keyI;
    keys[GLFW_KEY_J]                = ofs::keyCode::keyJ;
    keys[GLFW_KEY_K]                = ofs::keyCode::keyK;
    keys[GLFW_KEY_L]                = ofs::keyCode::keyL;
    keys[GLFW_KEY_M]                = ofs::keyCode::keyM;
    keys[GLFW_KEY_N]                = ofs::keyCode::keyN;
    keys[GLFW_KEY_O]                = ofs::keyCode::keyO;
    keys[GLFW_KEY_P]                = ofs::keyCode::keyP;
    keys[GLFW_KEY_Q]                = ofs::keyCode::keyQ;
    keys[GLFW_KEY_R]                = ofs::keyCode::keyR;
    keys[GLFW_KEY_S]                = ofs::keyCode::keyS;
    keys[GLFW_KEY_T]                = ofs::keyCode::keyT;
    keys[GLFW_KEY_U]                = ofs::keyCode::keyU;
    keys[GLFW_KEY_V]                = ofs::keyCode::keyV;
    keys[GLFW_KEY_W]                = ofs::keyCode::keyW;
    keys[GLFW_KEY_X]                = ofs::keyCode::keyX;
    keys[GLFW_KEY_Y]                = ofs::keyCode::keyY;
    keys[GLFW_KEY_Z]                = ofs::keyCode::keyZ;

    keys[GLFW_KEY_SPACE]            = ofs::keyCode::keySpace;
    keys[GLFW_KEY_SLASH]            = ofs::keyCode::keySlash;
    keys[GLFW_KEY_PERIOD]           = ofs::keyCode::keyPeriod;
    keys[GLFW_KEY_COMMA]            = ofs::keyCode::keyComma;
    keys[GLFW_KEY_MINUS]            = ofs::keyCode::keyMinus;
    keys[GLFW_KEY_EQUAL]            = ofs::keyCode::keyEqual;
    keys[GLFW_KEY_SEMICOLON]        = ofs::keyCode::keySemicolon;
    keys[GLFW_KEY_APOSTROPHE]       = ofs::keyCode::keyApostrophe;
    keys[GLFW_KEY_BACKSLASH]        = ofs::keyCode::keyBackslash;
    keys[GLFW_KEY_GRAVE_ACCENT]     = ofs::keyCode::keyGraveAccent;

    keys[GLFW_KEY_TAB]              = ofs::keyCode::keyTab;
    keys[GLFW_KEY_BACKSPACE]        = ofs::keyCode::keyBackspace;
    keys[GLFW_KEY_ENTER]            = ofs::keyCode::keyEnter;
    keys[GLFW_KEY_LEFT_BRACKET]     = ofs::keyCode::keyLeftBracket;
    keys[GLFW_KEY_RIGHT_BRACKET]    = ofs::keyCode::keyRightBracket;

    keys[GLFW_KEY_ESCAPE]           = ofs::keyCode::keyEscape;
    keys[GLFW_KEY_LEFT_ALT]         = ofs::keyCode::keyLeftAlt;
    keys[GLFW_KEY_LEFT_CONTROL]     = ofs::keyCode::keyLeftControl;
    keys[GLFW_KEY_LEFT_SHIFT]       = ofs::keyCode::keyLeftShift;
    keys[GLFW_KEY_RIGHT_ALT]        = ofs::keyCode::keyRightAlt;
    keys[GLFW_KEY_RIGHT_CONTROL]    = ofs::keyCode::keyRightControl;
    keys[GLFW_KEY_RIGHT_SHIFT]      = ofs::keyCode::keyRightShift;

    keys[GLFW_KEY_LEFT]             = ofs::keyCode::keyLeft;
    keys[GLFW_KEY_RIGHT]            = ofs::keyCode::keyRight;
    keys[GLFW_KEY_UP]               = ofs::keyCode::keyUp;
    keys[GLFW_KEY_DOWN]             = ofs::keyCode::keyDown;
    keys[GLFW_KEY_DELETE]           = ofs::keyCode::keyDelete;
    keys[GLFW_KEY_INSERT]           = ofs::keyCode::keyInsert;
    keys[GLFW_KEY_PAGE_UP]          = ofs::keyCode::keyPageUp;
    keys[GLFW_KEY_PAGE_DOWN]        = ofs::keyCode::keyPageDown;
    keys[GLFW_KEY_HOME]             = ofs::keyCode::keyHome;
    keys[GLFW_KEY_END]              = ofs::keyCode::keyEnd;
    keys[GLFW_KEY_CAPS_LOCK]        = ofs::keyCode::keyCapsLock;
    keys[GLFW_KEY_SCROLL_LOCK]      = ofs::keyCode::keyScrollLock;
    keys[GLFW_KEY_NUM_LOCK]         = ofs::keyCode::keyNumLock;
    keys[GLFW_KEY_PRINT_SCREEN]     = ofs::keyCode::keyPrintScreen;
    keys[GLFW_KEY_PAUSE]            = ofs::keyCode::keyPause;

    keys[GLFW_KEY_F1]               = ofs::keyCode::keyF1;
    keys[GLFW_KEY_F2]               = ofs::keyCode::keyF2;
    keys[GLFW_KEY_F3]               = ofs::keyCode::keyF3;
    keys[GLFW_KEY_F4]               = ofs::keyCode::keyF4;
    keys[GLFW_KEY_F5]               = ofs::keyCode::keyF5;
    keys[GLFW_KEY_F6]               = ofs::keyCode::keyF6;
    keys[GLFW_KEY_F7]               = ofs::keyCode::keyF7;
    keys[GLFW_KEY_F8]               = ofs::keyCode::keyF8;
    keys[GLFW_KEY_F9]               = ofs::keyCode::keyF9;
    keys[GLFW_KEY_F10]              = ofs::keyCode::keyF10;
    keys[GLFW_KEY_F11]              = ofs::keyCode::keyF11;
    keys[GLFW_KEY_F12]              = ofs::keyCode::keyF12;

    keys[GLFW_KEY_KP_0]             = ofs::keyCode::keyPad0;
    keys[GLFW_KEY_KP_1]             = ofs::keyCode::keyPad1;
    keys[GLFW_KEY_KP_2]             = ofs::keyCode::keyPad2;
    keys[GLFW_KEY_KP_3]             = ofs::keyCode::keyPad3;
    keys[GLFW_KEY_KP_4]             = ofs::keyCode::keyPad4;
    keys[GLFW_KEY_KP_5]             = ofs::keyCode::keyPad5;
    keys[GLFW_KEY_KP_6]             = ofs::keyCode::keyPad6;
    keys[GLFW_KEY_KP_7]             = ofs::keyCode::keyPad7;
    keys[GLFW_KEY_KP_8]             = ofs::keyCode::keyPad8;
    keys[GLFW_KEY_KP_9]             = ofs::keyCode::keyPad9;
    keys[GLFW_KEY_KP_DECIMAL]       = ofs::keyCode::keyPadDecimal;
    keys[GLFW_KEY_KP_DIVIDE]        = ofs::keyCode::keyPadDivide;
    keys[GLFW_KEY_KP_MULTIPLY]      = ofs::keyCode::keyPadMultiply;
    keys[GLFW_KEY_KP_ADD]           = ofs::keyCode::keyPadAdd;
    keys[GLFW_KEY_KP_SUBTRACT]      = ofs::keyCode::keyPadSubtract;
    keys[GLFW_KEY_KP_ENTER]         = ofs::keyCode::keyPadEnter;

}

void GUIManager::toggleFullScreen()
{

}

// Rendering GUI user interface
void GUIManager::render()
{
    gclient->startImGuiNewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


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

    char32_t ch = keys[gkey];

    // Logger::logger->debug("Code: {} -> {} action: {} mods: {}\n", gkey,  int(keys[gkey]), action, mods);

    // Process keyboard movement control
    ofsAppCore->keyPress(ch, 0, action == GLFW_PRESS);

    // Just ignore repeating action
    if (action == GLFW_REPEAT)
        return;

    // char32_t ch = keys[gkey];
    // ofsAppCore->keyEntered(key, 0);
}
