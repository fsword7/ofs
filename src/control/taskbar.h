// taskbar.h - Task Bar package
//
// Author:  Tim Stark
// Date:    Feb 23, 2024

#pragma once

class Panel;
class GraphicsClient;
class Player;
class Font;

class TaskBar
{
public:
    TaskBar(const Panel *panel);
    ~TaskBar();

    void update();

    void displayPlanetocentric(double lat, double lng, double alt);
    void displayPlanetInfo(const Player &player);

private:
    const Panel *panel = nullptr;
    GraphicsClient *gc = nullptr;

    Font *titleFont = nullptr;
    Font *textFont = nullptr;
};