// oglclient.cpp - OpenGL Graphics Client package
//
// Author:  Tim Stark
// Date:    Aug 30, 2022

#include "main/core.h"
#include "client.h"
#include "scene.h"

#include <dlfcn.h>

glClient *gclient = nullptr;
ModuleHandle myHandle = nullptr;
ofsLogger *logger = nullptr;

LIBCALL void initModule(ModuleHandle handle)
{
    logger = new ofsLogger(ofsLogger::logDebug, "glclient.log");
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


bool glClient::cbInitialize()
{
    // // Initialize GLEW package
    // GLenum err = glewInit();
    // if (err != GLEW_OK)
    // {
    //     logger->fatal("GLEW error: {}\n",
    //         glewGetErrorString(err));
    //     abort();
    // }

    // logger->info("Using GLEW version: {}\n", glewGetString(GLEW_VERSION));
    // logger->info("    OpenGL version: {}\n", glGetString(GL_VERSION));
    // // logger->info("Using Eigen version {}.{}\n",
    // //     EIGEN_MAJOR_VERSION, EIGEN_MINOR_VERSION);

    width  = 1920;
    height = 1080;

    return true;
}

void glClient::cbCleanup()
{

}

bool glClient::cbCreateRenderingWindow()
{
    window = SDL_CreateWindow("OFS" /* ofsGetAppShortName() */,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        // Logger::getLogger()->fatal("SDL2 Window can't be created: {}\n", SDL_GetError());
        printf("SDL2 Window can't be created: %s\n", SDL_GetError());
        abort();
    }
    SDL_ShowWindow(window);

    // Select OpenGL version before starting up
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    ctx = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    // Initialize GLEW package
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        logger->fatal("GLEW error: {:s}\n",
            (char *)glewGetErrorString(err));
        abort();
    }

    logger->info("Using GLEW version: {:s}\n", (char *)glewGetString(GLEW_VERSION));
    logger->info("    OpenGL version: {:s}\n", (char *)glGetString(GL_VERSION));
    // logger->info("Using Eigen version {}.{}\n",
    //     EIGEN_MAJOR_VERSION, EIGEN_MINOR_VERSION);

    // Initialize scene package
    scene = new Scene(width, height);
    scene->init();

    return true;
}

bool glClient::cbDisplayFrame()
{
    // Swap frame buffers
    SDL_GL_SwapWindow(window);

    // Clear all framebuffer
    // glClearColor(0.0, 0.0, 0.0, 1.0);
    // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    return true;
}

void glClient::cbStart()
{
    if (scene != nullptr)
        scene->start();
}

void glClient::cbRenderScene()
{
    if (scene != nullptr)
    {
        scene->update();
        scene->render();
    }
}