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

private:
    GraphicsClient *gclient = nullptr;
    GLFWwindow *window = nullptr;
};