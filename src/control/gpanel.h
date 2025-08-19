// gpanel.h - Generic panel package
//
// Author:  Tim Stark
// Date:    Aug 14, 2025

#pragma once

class Panel;
class Player; 

class GenericPanel : public PanelEx
{
    friend class Panel;
public:
    GenericPanel(Panel *panel);
    ~GenericPanel();

    void initResources();

    void render();
    void draw(Player &player, Sketchpad *pad) override;

    void drawButton(Sketchpad *pad, int x0, int y0, int x1, int y1, bool on);

protected:
    GraphicsClient *gc = nullptr;

    int width, height;
    int cx, cy;

    Brush *brushOn = nullptr;
    Brush *brushOff = nullptr;
    Pen *hudPen = nullptr;

    color_t pwrColor = { 1, 0, 0, .75 };
    color_t btnTextOn = { 1, 1, 1, .75 };
    color_t btnTextOff = { 1, 1, 1, .50 };
};