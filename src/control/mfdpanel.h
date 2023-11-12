// mfdpanel.h - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

class Panel;

class MFDInstrument
{
public:
    struct Spec
    {
        int w, h;           // MFD instrument display size
        int nbtl, nbtr;     // Number of buttons on left and right side of the display
        int bty0, btdy;     // offset of top button and distance between buttons
        uint32_t flags;
    };

    MFDInstrument(Panel *panel, const Spec &spec);
    virtual ~MFDInstrument() = default;

    static MFDInstrument *create(Panel *panel, const Spec &spec);

    void init(const Spec &spec);
    Sketchpad *beginDraw();
    void endDraw(Sketchpad *skpad);

    // Virtual function calls
    virtual bool update();
    virtual void draw(Sketchpad *skpad) = 0;

private:
    Panel *panel = nullptr;
    GraphicsClient *gc = nullptr;
    Sketchpad *skpad = nullptr;

    int iWidth, iHeight;        // MFD instrument display size
    int nbtl, nbtr;             // Number buttons on left and right side of the display
    int nbt;                    // Total number of buttons
    int bty0, btdy;             // offset of top button and distance between buttons

    uint32_t flags;
};