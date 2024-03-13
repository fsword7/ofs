// mfdpanel.h - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#pragma once

class Vehicle;
class Panel;
class Pen;

struct MFDSpec
{
    int w, h;           // MFD instrument display size
    int nbtl, nbtr;     // Number of buttons on left and right side of the display
    int bty0, btdy;     // offset of top button and distance between buttons
    uint32_t flags;
};

struct DrawColor
{
    color_t color;
    Pen *solidPen = nullptr;
    Pen *dashedPen = nullptr;
};

// Orbital elements data for orbit path graph
#define NOREL   512
#define NOREL2  (NOREL/2)
#define NOREL4  (NOREL/4)

class MFDInstrument
{
public:

    MFDInstrument(Panel *panel, const MFDSpec &spec, Vehicle *vehicle);
    virtual ~MFDInstrument() = default;

    static MFDInstrument *create(Panel *panel, const MFDSpec &spec);
    static void ginit(GraphicsClient *gc);
    static void gexit(GraphicsClient *gc);

    void init(const MFDSpec &spec);
    Sketchpad *beginDraw();
    void endDraw(Sketchpad *skpad);
    cchar_t *getButtonLabel(int idx);

    // Virtual function calls
    virtual bool update();
    virtual void draw(Sketchpad *skpad) = 0;

    virtual cchar_t *mfdGetButtonLabel(int idx) { return nullptr; }

protected:
    glm::dmat3 getRotMatrix(double coso, double sino, double cosp, double sinp, double cosi, double sini);
    glm::dmat3 getInverseRotMatrix(double cosp, double sinp, double cosi, double sini);
    void mapScreen(int xCenter, int yCenter, double scale, const glm::dvec3 &vtx, glm::dvec2 *pt);
    void updateOrbitPath(int xCenter, int yCenter, int iWidth, int iHeight, double scale, const OrbitalElements &orbit,
        const glm::dmat3 &rot, const glm::dmat3 &irot, glm::dvec2 *pt);
    void updateEllipse(int xCenter, int yCenter, double scale, const OrbitalElements &orbit,
        const glm::dmat3 &rot, const glm::dmat3 &irot, glm::dvec2 *pt);
    void updateHyperboa(int xCenter, int yCenter, int iWidth, int iHeight, double scale, const OrbitalElements &orbit,
        const glm::dmat3 &rot, const glm::dmat3 &irot, glm::dvec2 *pt);

protected:
    Panel *panel = nullptr;
    Vehicle *vehicle = nullptr;
    GraphicsClient *gc = nullptr;
    Sketchpad *skpad = nullptr;

    int iWidth, iHeight;        // MFD instrument display size
    int cw, ch;                 // Character width/height of spedific font
    int nbtl, nbtr;             // Number buttons on left and right side of the display
    int nbt;                    // Total number of buttons
    int bty0, btdy;             // offset of top button and distance between buttons

    uint32_t flags;

    static DrawColor drawColors[3][2];   
};