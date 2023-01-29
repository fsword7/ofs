// guimgr.h - GUI manager package
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include <GLFW/glfw3.h>

class GUIManager
{
public:
    GUIManager(GraphicsClient *gclient);
    ~GUIManager();

    void setupCallbacks();

    bool shouldClose();
    void pollEvents();

    void processFramebufferSize(GLFWwindow *window, int w, int h);
    void processScroll(GLFWwindow *window, double xoff, double yoff);
    void processCursorPosition(GLFWwindow *window, double xpos, double ypos);
    void processMouseButton(GLFWwindow *window, int button, int action, int mods);
    void processKey(GLFWwindow *window, int gkey, int scancode, int action, int mods);

private:
    GraphicsClient *gclient = nullptr;
    GLFWwindow *window = nullptr;
};