// guimgr.h - GUI manager package
//
// Author:  Tim Stark
// Date:    Jan 28, 2023

#include <GLFW/glfw3.h>

class GUIManager
{
public:
    GUIManager() = default;
    ~GUIManager() = default;

    void init();
    void cleanup();

private:
};