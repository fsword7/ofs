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
#include "surface.h"
#include "vobject.h"
#include "vbody.h"

#include <dlfcn.h>

glClient *gclient = nullptr;
ModuleHandle myHandle = nullptr;
ofsLogger *glLogger = nullptr;

LIBCALL void initModule(ModuleHandle handle)
{
    glLogger = new ofsLogger(ofsLogger::logDebug, "client.log");
    glLogger->info("--------- OpenGL client --------\n");

    gclient = new glClient(handle);
    if (!ofsRegisterGraphicsClient(gclient))
    {
        printf("Can't register graphics client - aborted.\n");
        delete gclient, glLogger;
        gclient = nullptr;
        glLogger = nullptr;
    }
}

LIBCALL void exitModule(ModuleHandle handle)
{
    if (gclient != nullptr)
    {
        ofsUnregisterGraphicsClient(gclient);
        delete gclient, glLogger;
        gclient = nullptr;
        glLogger = nullptr;
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
    if (glLogger != nullptr)
        glLogger->fatal("GLFW Error: {}\n", errMessage);
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

    glLogger->info("Loaded OpenGL version: {}.{}\n",
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

Texture *glClient::createSurface(int width, int height, uint32_t flags)
{
    TextureManager mgr;

    return mgr.getTextureForRendering(width, height, flags);
}

void glClient::releaseSurface(Texture *surf)
{
    delete surf;
}

void glClient::clearSurface(Texture *surf, const color_t &color)
{
    // Make ensure that frame buffer is not binded
    GLint fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
    assert(fbo == 0);

    // Clear entire surface with specific color
    glBindFramebuffer(GL_FRAMEBUFFER, surf->fbo);
    glClearColor(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void glClient::cbRenderScene(Player *player)
{
    if (scene != nullptr)
    {
        scene->update(player);
        scene->render(player);
    }
}

double glClient::getElevationData(CelestialBody *cbody, glm::dvec3 loc, int reqlod,
    elevTileList_t *tiles, glm::dvec3 *normal, int *lod)
{
    vBody *vobj = dynamic_cast<vBody *>(cbody->getVisualObject());
    assert(vobj != nullptr);
    SurfaceManager *smgr = vobj->getSurfaceManager();

    return smgr->getElevationData(loc, reqlod, tiles, normal, lod);
}

Font *glClient::createFont(int height, bool fixed, cchar_t *face, Font::Style style, int orientation, bool antialiased)
{
    return new glFont(height, fixed, face, style, orientation, antialiased);
}


Pen *glClient::createPen(color_t color, int width, int style)
{
    return new glPen(color, width, style);
}

Brush *glClient::createBrush(color_t color)
{
    return new glBrush(color);
}

void glClient::releaseFont(Font *font)
{
    delete (glFont *)font;
}

void  glClient::releasePen(Pen *pen)
{
    delete (glPen *)pen;
}

void glClient::releaseBrush(Brush *brush)
{
    delete (glBrush *)brush;
}