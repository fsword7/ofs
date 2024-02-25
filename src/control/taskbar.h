// taskbar.h - Task Bar package
//
// Author:  Tim Stark
// Date:    Feb 23, 2024

#pragma once

class Panel;
class GraphicsClient;

class TaskBar
{
public:
    TaskBar(const Panel *panel);
    ~TaskBar();

    void update();

private:
    const Panel *panel = nullptr;
    GraphicsClient *gc = nullptr;
};