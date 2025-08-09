// taskbar.h - Task Bar package
//
// Author:  Tim Stark
// Date:    Feb 23, 2024

#pragma once

class OrbitalElements;
class Panel;
class GraphicsClient;
class Sketchpad;
class Player;
class Font;

class TaskBar
{
public:
    TaskBar(const Panel *panel);
    ~TaskBar();

    void resize();

    void update(const Player &player, double simt);
    void render(const Player &player);

    void initPlanetInfo();
    void cleanPlanetInfo();
    void displayPlanetocentric(double lat, double lng, double alt);
    void displayOrbitalElements(const OrbitalElements &oel, double rad);
    void displayPlanetInfo(const Player &player);

private:
    const Panel *panel = nullptr;
    GraphicsClient *gc = nullptr;
    int height, width;

    Sketchpad *ipad = nullptr;
    Font *titleFont = nullptr;
    Font *textFont = nullptr;
};