// oglclient.cpp - OpenGL Graphics Client package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#include "main/core.h"
#include "engine/player.h"
#include "universe/universe.h"
#include "client.h"
#include "texmgr.h"
#include "skpad.h"
#include "scene.h"

#include <dlfcn.h>

glClient *gclient = nullptr;
ModuleHandle myHandle = nullptr;
ofsLogger *logger = nullptr;

LIBCALL void initModule(ModuleHandle handle)
{
    logger = new ofsLogger(ofsLogger::logDebug, "client.log");
    logger->info("--------- OpenGL client --------\n");

    gclient = new glClient(handle);
    if (!ofsRegisterGraphicsClient(gclient))
    {
        printf("Can't register graphics client - aborted.\n");
        delete gclient, logger;
        gclient = nullptr;
        logger = nullptr;
    }
}

LIBCALL void exitModule(ModuleHandle handle)
{
    if (gclient != nullptr)
    {
        ofsUnregisterGraphicsClient(gclient);
        delete gclient, logger;
        gclient = nullptr;
        logger = nullptr;
    }
}

static void __attribute__ ((constructor)) setupModule(void)
{
    printf("OpenGL client module loaded.\n");

    Dl_info info;
    int ret = dladdr((void *)setupModule, &info);
    if (ret == 0)
    {
        printf("dladdr failed - aborted\n");
        assert(false);
    }
    // void (*initModule)(ModuleHandle) =
    //     (void(*)(ModuleHandle))ofsGetProcAddress(info.dli_fbase, "initModule");
    // printf("initModule - %p (%p)\n", initModule, info.dli_fbase);
    myHandle = info.dli_fbase;
    initModule(myHandle);
}

static void __attribute__ ((destructor)) destroyModule(void)
{
    exitModule(myHandle);
    printf("OpenGL client module unloaded.\n");
}

void cbPrintError(int, cchar_t *errMessage)
{
    printf("GLFW Error: %s\n", errMessage);
    if (logger != nullptr)
        logger->fatal("GLFW Error: {}\n", errMessage);
}

bool glClient::cbInitialize()
{
    width  = SCR_WIDTH;
    height = SCR_HEIGHT;

    glfwSetErrorCallback(cbPrintError);

    // overlay = new Overlay(*ctx);
    // titleFont = TextureFont::load(*ctx, "fonts/OpenSans-Bold.ttf", 20);
    // textFont = TextureFont::load(*ctx, "fonts/OpenSans-Regular.ttf", 12);

    return true;
}

void glClient::cbCleanup()
{
    if (window != nullptr)
        glfwDestroyWindow(window);
    window = nullptr;

    // Release GLFW inteface
    glfwTerminate();
}

GLFWwindow *glClient::cbCreateRenderingWindow()
{
    // Set OpenGL core profile request
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
    // Set OpenGL version request
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(width, height, "OFS", NULL, NULL);
    if (window == nullptr)
    {
        printf("GLFW Window can't be created");
        abort();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize OpenGL interface
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        printf("GLAD: Failed to initialize OpenGL interface - aborted\n");
        abort();
    }

    logger->info("Loaded OpenGL version: {}.{}\n",
        GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    // Initialize scene package
    scene = new Scene(width, height);
    // scene->init();

    ofsInitGLFW(window);
    ImGui_ImplOpenGL3_Init("#version 430");

    return window;
}

void glClient::showWindow()
{
    glfwShowWindow(window);
}

void glClient::hideWindow()
{
    glfwHideWindow(window);
}

void glClient::startImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
}

void glClient::renderImGuiDrawData()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();
}

bool glClient::cbDisplayFrame()
{
    // Swap frame buffers
    glfwSwapBuffers(window);

    // Clear all framebuffer
    // glClearColor(0.0, 0.0, 0.0, 1.0);
    // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    return true;
}

void glClient::cbSetWindowTitle(cstr_t &title)
{
    glfwSetWindowTitle(window, title.c_str());
}

void glClient::setViewportSize(int w, int h)
{
    // Update resolution
    width = w;
    height = h;

    if (scene != nullptr)
        scene->resize(width, height);
}

void glClient::cbStart(Universe *universe)
{
    if (scene != nullptr)
    {
        scene->init(universe);
        scene->start();
    }
}

void glClient::loadTextureFont()
{

}

void glClient::cbRenderScene(Player *player)
{
    if (scene != nullptr)
    {
        scene->update(player);
        scene->render(player);
    }
}

Texture *glClient::createSurface(int width, int height, uint32_t flags)
{
    TextureManager mgr;

    return mgr.getTextureForRendering(width, height, flags);
}

void glClient::releaseSurface(Texture *surf)
{
    delete surf;
}