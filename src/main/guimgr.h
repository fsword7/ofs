// guimgr.h - GUI manager package
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include <GLFW/glfw3.h>

#define OFSAPI_SERVER_BUILD
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>

class GUIManager
{
public:
    GUIManager(GraphicsClient *gclient);
    ~GUIManager();

    void resetKeys();

    void toggleFullScreen();

    void render();

    void setupCallbacks();

    bool shouldClose();
    void pollEvents();

    void processFramebufferSize(GLFWwindow *window, int w, int h);
    void processMouseWheel(GLFWwindow *window, double xoff, double yoff);
    void processMousePosition(GLFWwindow *window, double xpos, double ypos);
    void processMouseButton(GLFWwindow *window, int button, int action, int mods);
    void processKey(GLFWwindow *window, int gkey, int scancode, int action, int mods);

private:
    GraphicsClient *gclient = nullptr;
    GLFWwindow *window = nullptr;

    // Save previous callback functions 
    void (*pcbProcessMouseWheel)(GLFWwindow *window, double xoff, double yoff);
    void (*pcbProcessMouseButton)(GLFWwindow *window, int button, int action, int mods);
    void (*pcbProcessMousePosition)(GLFWwindow *window, double xpos, double ypos);
    void (*pcbProcessKey)(GLFWwindow *window, int gkey, int scancode, int action, int mods);

    // GLFW/OFS Key mapping table
    char32_t keys[GLFW_KEY_LAST];
};